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

void write_to(int fd,char* msg){
        char wbuf[4+MAX_BUF] = {};
        int sz = strlen(msg);
        memcpy(wbuf+4,msg,sz);
        write_len(sz,wbuf);

        int n = write_all(fd, wbuf,4+sz);
}

int read_from(int fd){

        char rbuf[MAX_BUF] = {};
        int len = parse_len(fd);
        if(len<0){
                return -1;
        }
        assert(len<MAX_BUF);
        int n = read_all(fd, rbuf,len);
        if(n<0){
                return -1;
        }

        printf("Server says: %s\n",rbuf);
        return 0;
}
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
                        read_from(fd);
                }
                else{
                        std::cout<<"connection closed\n";
                        break;
                }
        }
}
