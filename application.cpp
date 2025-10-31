#include "struct.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <stdio.h>
#include <arpa/inet.h>

using std::string;
const int buf_size = 1024;

void make_non_block(int fd){
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL,0) | O_NONBLOCK);
}

int get_len(uint8_t* ch){
        int res = 0;
        for(int i = 0;i<4;i++){
                res += (ch[i]<<(i*8));
        }
        return res;
}

string parse(vector<uint8_t>& read){
        string res{};
        if(read.size()<4){
                return res;
        }
        int len = get_len(read.data());
        if(len+4 <= read.size()){
                res.insert(res.end(),read.begin()+4,read.begin()+4+len);
        }
        return res;
}

void put_len(int len, vector<uint8_t>& res){
        for(int i = 0;i<4;i++){
                res[i] = (uint8_t)((len<<24)>>24);
                len>>=8;
        }
}

vector<uint8_t> get_res(string str){
        vector<uint8_t>res(4,0);
        put_len(str.size(),res);
        res.insert(res.end(),str.begin(),str.end());
        return res;
}

void process_payload(string& str,Conn* con){
        auto&read = con->read;
        con->want_write = true;
        read.erase(read.begin(),read.begin()+(4+str.size()));
        if(str == "exit"){
                con->want_read = false;
                con->want_close = true;
                return;
        }
        printf("%s:%d says: %s\n",con->addr,con->port,str.data());
        con->want_write = true;
        auto s = get_res(string("Hello")+' '+string(con->addr)+':'+std::to_string(con->port));
        auto& write = con->write;
        write.insert(write.end(),s.begin(),s.end());
}

Conn* handle_accept(int fd){
        struct sockaddr_in addr = {};
        socklen_t len = sizeof(addr);
        make_non_block(fd);
        int c_fd = accept(fd, (struct sockaddr*)&addr,&len);
        if(c_fd<0){
                perror("Can't accept.");
                return nullptr;
        }

        Conn* con = new(Conn);
        make_non_block(c_fd);
        con->fd = c_fd;
        con->want_read = true;
        char* s_addr = new char[INET_ADDRSTRLEN+1];
        s_addr[INET_ADDRSTRLEN] = 0;
        inet_ntop(AF_INET,&(addr.sin_addr),s_addr,INET_ADDRSTRLEN);
        con->addr = s_addr;
        con->port = addr.sin_port;
        printf("New connection stablished with: %s:%d\n",con->addr,con->port);
        return con;
}

void handle_read(Conn* con){
        char rbuf[buf_size]={};
        int n = read(con->fd, &rbuf,sizeof(buf_size)-1);
        if(n<0){
                perror("Unable to read.");
                con->want_close = true;
                return;
        }

        con->read.insert(con->read.end(),rbuf,rbuf+n);
        
        string payload = parse(con->read);
        while(payload.size()){
                process_payload(payload,con);
                payload = parse(con->read);
        }
}

void handle_write(Conn* con){
        auto&write_buf = con->write;
        if(write_buf.size()){
                int n = write(con->fd,con->write.data(),con->write.size());
                if(n<0){
                        if(errno == EINTR)return;
                        perror("Unable to write");
                        con->want_close = true;
                        con->want_write = false;
                        con->want_read = false;
                        return;
                }
                auto&write = con->write;
                write.erase(write.begin(),write.begin()+n);
                if(!write.size()){
                        con->want_write = false;
                }
        }
        else{
                con->want_write = false;
        }
}
