#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include "format.cpp"

static const int MAX_BUF = 4096;

int talk(int fd,const char* msg){
        char wbuf[4+MAX_BUF] = {};
        int sz = strlen(msg);
        memcpy(wbuf+4,msg,sz);
        write_len(sz,wbuf);

        int n = write_all(fd, wbuf,4+sz);
        if(n<0){
                return -1;
        }

        char rbuf[MAX_BUF] = {};
        int len = parse_len(fd);
        if(len<0){
                return -1;
        }

        n = read_all(fd, rbuf,len);
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
