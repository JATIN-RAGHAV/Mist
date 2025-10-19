#include <sys/poll.h>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <unistd.h>

struct conn{
        int fd = -1;
        bool want_read = false;
        bool want_write = false;
        bool want_close = false;
        std::vector<uint8_t> incoming;
        std::vector<uint8_t> outgoing;
};

conn* handle_accept(int fd){
        struct sockaddr_in c_addr = {};
        socklen_t len = sizeof(c_addr);
        int c_fd = accept(fd,(struct sockaddr*)&c_addr, &len);
        conn* new_conn = new(conn);
        new_conn->fd = c_fd;
        new_conn->want_read = true;
        return new_conn;
}

void handle_read(conn* con);
void handle_write(conn* con);

static int on = 1,off = 0;
int main(){
        int fd = socket(AF_INET,SOCK_STREAM,0);

        setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

        struct sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(1234);
        addr.sin_addr.s_addr = htonl(0);
        int n = bind(fd,(const struct sockaddr*)&addr,sizeof(addr));
        if(n){
                perror("Can't bind.");
        }

        n = listen(fd,20);
        if(n){
                perror("Can't listen.");
        }

        std::vector<conn*> fds;
        std::vector<struct pollfd> poll_args;

        // Event Loop
        while(true){
                poll_args.clear();

                struct pollfd pfd = {fd,POLLIN,0};
                poll_args.push_back(pfd);
                
                for(conn* con:fds){
                        if(!con){
                                continue;
                        }
                        struct pollfd pfd = {con->fd,POLLERR,0};
                        if(con->want_read){
                                pfd.events|=POLLIN;
                        }
                        if(con->want_write){
                                pfd.events|=POLLOUT;
                        }
                        poll_args.push_back(pfd);
                }

                int ret = poll(poll_args.data(),poll_args.size(),-1);
                if(ret < 0 && errno == EINTR){
                        continue;
                }
                if(ret < 0){
                        perror("poll gave us an error.");
                }

                if(poll_args[0].revents == POLLIN){
                        if(conn* con = handle_accept(poll_args[0].fd)){
                                if(fds.size()-1<con->fd){
                                        fds.resize(con->fd+1);
                                }
                                fds[con->fd] = con;
                        }
                }
                
                for(int i = 1;i<poll_args.size();i++){
                        conn* con = fds[poll_args[i].fd];
                        int result = poll_args[i].revents;
                        if(result & POLLIN){
                                handle_read(con);
                        }
                        if(result & POLLOUT){
                                handle_write(con);
                        }


                        if((result & POLLERR) || con->want_close){
                                close(con->fd);
                                fds[poll_args[i].fd] = NULL;
                                delete con;
                        }
                }
        }
}
