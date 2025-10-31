#include <cassert>
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "common.cpp"
#include "debug.cpp"

const int PORT = 4237;
void run_client();

int main(){
        run_client();
}

void run_client(){
        int fd = socket(AF_INET,SOCK_STREAM,0);

        struct sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        int n = connect(fd,(struct sockaddr*)&addr, sizeof(addr));
        if(n < 0){
                std::cerr<<"Can't connect to the client.\n";
                exit(1);
        }

        std::cout << "Connection stablished!!\n"
                     "Start giving in instructions now. OK??\n"
                     "Following are the only valid instructions\n"
                     "set key value\n"
                     "get key\n"
                     "del key\n\n"
                     "Enter: 'quit' to quit\n\n";

        while(true){
                std::cout<<"\n\n>";
                std::string tot_len{4,' '};
                std::string cmd_str;
                std::getline(std::cin, cmd_str);
                if(cmd_str == "quit"){
                        std::cout << "Ending connection.\n"
                                     "Byeee!!!\n\n";
                        exit(0);
                }
                std::vector<uint8_t> cmd = make_cmd(cmd_str);
                while(cmd.size()){
                        int n = write(fd,cmd.data(),cmd.size());
                        if(n<0){
                                std::cerr << "Unable to write to the server.";
                                exit(1);
                        }
                        assert(n<=cmd.size());
                        cmd.erase(cmd.begin(),cmd.begin()+n);
                }
                std::vector<uint8_t> res;
                while(true){
                        char reading[64*1024]={};
                        errno = 0;
                        int n = read(fd,reading,sizeof(reading));
                        if(n<=0 && (errno& EINTR)){
                                continue;
                        }
                        if(n<=0){
                                std::cout<<"Maybe the server closed the connection.\n";
                                exit(1);
                        }

                        res.insert(res.end(),reading,reading+n);
                        std::cout<<std::endl;
                        if(res.size()>=4){
                                int len = get_len(res.begin());
                                if(len+4 <= res.size()){
                                        std::cout << "Server says: " << res.data()+4 <<'\n';
                                        break;
                                }
                        }
                }
        }

}
