#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include "format.cpp"
#include <chrono>
#include <random>
#include <thread>

void test();

int main(){
        for(int i = 0;i<20;i++){
                std::thread new_thread(test);
                new_thread.detach();
        }
        sleep(30);
}
int i = 0;

void test(){
        int fd = socket(AF_INET,SOCK_STREAM,0);
        struct sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(1234);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        std::vector<std::string> msgs = {"first","second","Third.","Fourth","exit"};
        std::mt19937 eng(std::chrono::high_resolution_clock::now().time_since_epoch().count()+i++);
        std::uniform_int_distribution<int> dist(1,2);
        sleep(dist(eng));
        int n = connect(fd, (struct sockaddr*)&addr,sizeof(addr));
        if(n){
                perror("Can't connect to the server.");
                exit(1);
        }

        for(auto s:msgs){
                sleep(dist(eng));
                int n = write_to(fd,s);
                if(n){
                        perror("Can't write the whole thing.");
                        exit(1);
                }
        }
        for(auto s:msgs){
                sleep(dist(eng));
                if(s=="exit"){
                        printf("Connection closed now.\n");
                        break;
                }
                char rbuf[MAX_BUF]={};
                int n = read_from(fd,rbuf);
                if(n){
                        perror("Can't read all the requests.");
                        exit(1);
                }
                printf("Server says: %s\n",rbuf);
        }
}
