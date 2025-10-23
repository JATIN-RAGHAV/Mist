#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include "format.cpp"
#include <string>

int talk(int fd,std::string msg){
        write_to(fd,msg);

        char rbuf[MAX_BUF] = {};
        read_from(fd,rbuf);
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
                perror("Couldn't connect to the server.\n");
        }
        
        n = talk(fd,"first hello, from client side.");
        if(n){
                perror("Unalbe to talk to the server.");
                goto end;
        }

        n = talk(fd,"second hello, from client side.");
        if(n){
                perror("Unalbe to talk to the server.");
                goto end;
        }
        end:
        close(fd);
}
