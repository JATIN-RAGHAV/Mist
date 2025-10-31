#include <cassert>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <cctype>
#include "common.cpp"
#include "data.cpp"
#include "debug.cpp"

#define MAX 4096
const std::string empty_sz = std::string(4,' ');

void kill_this(Conn* con){
        con->want_close = true;
        con->want_read = false;
        con-> want_write = false;
        con->writer.clear();
        con->reader.clear();
}

void gen_response(std::string res,Conn* con){
        auto& writer = con->writer;

        writer.insert(writer.end(),empty_sz.begin(),empty_sz.end());
        set_len(res.size(),writer.end()-4);

        writer.insert(writer.end(),res.begin(),res.end());
        con->want_write = true;
}

void execute_command(std::vector<std::string>& cmd,Conn* con){
        std::transform(cmd[0].begin(),cmd[0].begin(),cmd[0].end(),::tolower);
        std::cout<<"Handling a " <<cmd[0] <<" request from: " <<con->ip<<':'<<con->port<<'\n';
        if(cmd.size() == 2){
                if(cmd[0] == "get"){
                        if(data.count(cmd[1]+con->ip)){
                                gen_response(data[cmd[1]+con->ip],con);
                        }
                        else{
                                kill_this(con);
                        }
                }
                else if(cmd[0] == "del"){
                        data.erase(cmd[1]+con->ip);
                        gen_response("done",con);
                }
                else{
                        kill_this(con);
                }
                return;
        }
        else if(cmd.size() == 3 && cmd[0] == "set"){
                data[cmd[1]+con->ip] = cmd[2];
                gen_response("done",con);
        }
        else{
                kill_this(con);
        }
}

void process_request(Conn* con){
        auto& reader = con->reader;
        int arg_cnt = get_len(reader.begin());
        std::vector<std::string>cmd;
        auto it = reader.begin()+4;
        while(it < reader.end()){
                int len = get_len(it);
                it+=4;
                cmd.push_back(std::string{it,it+len});
                it+=len;
        }
        execute_command(cmd,con);
}

Conn* handle_accept(int fd){
        struct sockaddr_in addr = {};
        socklen_t len = sizeof(addr);
        int c_fd = accept(fd, (struct sockaddr*)&addr, &len);
        if(c_fd < 0){
                return NULL;
        }
        Conn* con = new(Conn);
        con->fd = c_fd;
        con->want_read = true;
        con->port = ntohs(addr.sin_port);

        char* ip = new(char[INET_ADDRSTRLEN+1]);
        ip[INET_ADDRSTRLEN] = 0;
        if(inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN)){
                con->ip = std::string(ip);
        }
        else{
                con->ip = std::string{":Couldn't get ip:"};
        }
        return con;
}

void handle_read(Conn* con){
        char rbuf[MAX] = {};
        errno = 0;
        int n = read(con->fd,&rbuf,MAX);
        if(n<0 && errno&EINTR){
                return;
        }
        if(n<0){
                kill_this(con);
                return;
        }
        
        auto& reader = con->reader;
        reader.insert(reader.end(),rbuf,rbuf+n);

        if(reader.size()>=4){
                int len = get_len(reader.begin());
                if(reader.size()-4 >= len){
                        process_request(con);
                        reader.erase(reader.begin(),reader.begin()+4+len);
                }
        }
}

void handle_write(Conn* con){
        auto&writer = con->writer;
        errno = 0;
        int n = write(con->fd,writer.data(),writer.size());
        if(n < 0 && errno&EINTR){
                return;
        }
        if(n < 0){
                kill_this(con);
                return;
        }

        assert(n <= writer.size());

        writer.erase(writer.begin(),writer.begin()+n);
}
