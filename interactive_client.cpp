#include <cstdlib>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <iostream>
#include "format.cpp"

#define MAX_BUF 4096
using std::string;

int main(){
        int fd = socket(AF_INET,SOCK_STREAM,0);

        struct sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(1234);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        
         int n = connect(fd,(const struct sockaddr*)&addr,sizeof(addr));
         if(n){
                perror("Unable to connect.");
                exit(1);
         }

        while(true){
                string str;
                std::cout<<"Enter you message:";
                getline(std::cin,str);
                write_to(fd,str.data());
                if(str != "exit"){
                        char rbuf[MAX_BUF] = {};
                        read_from(fd,rbuf);
                        printf("Server says: %s\n",rbuf);
                }
                else{
                        std::cout<<"connection closed\n";
                        break;
                }
        }
}
