#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>
#include "struct.hpp"
#include "application.cpp"

using std::vector;

int main(){
        int fd = socket(AF_INET, SOCK_STREAM,0);
        int on = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,&on,sizeof(on));

        struct sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(1234);
        addr.sin_addr.s_addr = htonl(0);
        int n = bind(fd,(const struct sockaddr*)&addr, sizeof(addr));
        if(n){
                perror("Couldn't bind.");
        }

        n = listen(fd,20);
        if(n){
                perror("Couldnt' listen.");
        }
        printf("Server is listening.\n");
        
        vector<Conn*> fds;
        vector<struct pollfd> poll_list;
        while(true){
                poll_list.clear();
                struct pollfd str {fd,POLLIN,0};
                poll_list.push_back(str);
                for(int i =0;i<fds.size();i++){
                        if(fds[i]){
                                struct pollfd str = {fds[i]->fd,POLLERR,0};
                                if(fds[i]->want_read){
                                        str.events|=POLLIN;
                                }
                                if(fds[i]->want_write){
                                        str.events|=POLLOUT;
                                }
                                poll_list.push_back(str);
                        }
                }

                int n = poll(poll_list.data(),poll_list.size(),-1);
                if(n<0 && errno == EINTR){
                        continue;
                }
                if(n<0){
                        perror("Poll returned an error.");
                        exit(1);
                }

                if(poll_list[0].revents){
                        if(Conn* con = handle_accept(fd)){
                                if(fds.size() <= con->fd){
                                        fds.resize(con->fd+1);
                                }
                                fds[con->fd] = con;
                        }
                }
                for(int i = 1; i<poll_list.size();i++){
                        int result = poll_list[i].revents;
                        if(result & POLLIN){
                                handle_read(fds[poll_list[i].fd]);
                        }
                        if(result & POLLOUT){
                                handle_write(fds[poll_list[i].fd]);
                        }

                        int s = poll_list[i].fd;
                        if(fds[s]->want_close || (poll_list[i].revents & POLLERR)){
                                close(s);
                                delete fds[s];
                                fds[s] = NULL;
                                printf("Client wants to close the connection.\n");
                                fflush(stdout);
                        }
                }
        }
}
