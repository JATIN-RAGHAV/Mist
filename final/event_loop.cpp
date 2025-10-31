#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <vector>
#include <poll.h>
#include <iostream>
#include <unistd.h>
#include "common.cpp"
#include "application.cpp"

const int PORT = 4237;

int main(){
        int fd = socket(AF_INET,SOCK_STREAM,0);

        int on = 1;
        setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

        struct sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(4237);
        addr.sin_addr.s_addr = htonl(0);

        int n = bind(fd,(const struct sockaddr*)&addr, sizeof(addr));
        if(n){
                std::cerr << "Something went wrong while attempting to bind.";
                exit(1);
        }

        n = listen(fd,SOMAXCONN);
        if(n){
                std::cerr <<"Something went wront while attempting to listen.";
                exit(1);
        }
        
        std::cout << "Server started to listen!!!\n";

        std::vector<Conn*> fds;
        std::vector<struct pollfd> poll_args;
        while(true){
                poll_args.clear();
                struct pollfd server = {fd,POLLIN,0};
                poll_args.push_back(server);

                for(int i = 0;i<fds.size();i++){
                        if(!fds[i])continue;
                        struct pollfd client = {fds[i]->fd,POLLERR,0};
                        if(fds[i]->want_read){
                                client.events |= POLLIN;
                        }
                        if(fds[i]->want_write){
                                client.events |= POLLOUT;
                        }
                        poll_args.push_back(client);
                }

                int n = poll(poll_args.data(),poll_args.size(),-1);
                if(n<0 && errno&EINTR){
                        continue;
                }
                if(n<0){
                        std::cout << "Something went wrong with polling.";
                        exit(1);
                }

                if(POLLIN & poll_args[0].revents){
                        if(Conn* con = handle_accept(fd)){
                                if(fds.size() <= con->fd){
                                        fds.resize(con->fd+1);
                                }
                                fds[con->fd] = con;
                                std::cout << "New connection stablished with: " << con->ip << ':' << con->port << ".\n";
                        }
                }

                for(int i = 1;i<poll_args.size();i++){
                        int result = poll_args[i].revents;
                        Conn* con = fds[poll_args[i].fd];
                        if(result & POLLIN){
                                handle_read(con);
                        }
                        if(result & POLLOUT){
                                handle_write(con);
                        }
                        
                        if((result & POLLERR) || con->want_close || (result & POLLHUP)){
                                close(con->fd);
                                std::cout << "Closing connection with: " << con->ip << ':' << con->port << ".\n";
                                fds[con->fd] = NULL;
                                free(con);
                        }

                }
        }
}
