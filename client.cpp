#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>


int main(){
        int fd = socket(AF_INET,SOCK_STREAM,0);
        
        struct sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(1234);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        int n = connect(fd, (const struct sockaddr*)&addr, sizeof(addr));
        if(n){
                perror("Couldn't connect to the server\n");
        }

        char wbuf[64] = "Hello, the client here.";
        n = write(fd,&wbuf,sizeof(wbuf));
        if(n<0){
                perror("Couldn't write to the server.");
        }

        char rbuf[64] = {};
        n = read(fd,&rbuf,sizeof(rbuf));
        if(n<0){
                perror("Couldn't read from the server.");
        }

        printf("Server says: %s\n",rbuf);
        close(fd);
}
