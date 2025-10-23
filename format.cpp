#include <cassert>
#include <string>
#include <unistd.h>
#include <cstring>
#include <iostream>

const int len_size = 4;
const int MAX_BUF = 64*1048;

static int read_all(int fd,char* buf,int n){
        while(n>0){
                int rv = read(fd,buf,n);
                if(rv<=0){
                        return -1;
                }

                assert(rv<=n);

                buf += rv;
                n-=rv;
        }
        return 0;
}

static int write_all(int fd,char* buf,int n){
        while(n>0){
                ssize_t rv = write(fd,buf,n);
                if(rv<=0){
                        return -1;
                }

                assert(rv<=n);

                buf += rv;
                n -= rv;
        }
        return 0;
}

static int parse_len(int fd){
        char len_buf[len_size] = {};
        int ans = 0;
        int n = read_all(fd,len_buf,4);
        if(n){
                perror("Can't read length\n");
                exit(1);
        }
        for(int i = 0;i<len_size;i++){
                char c = len_buf[i];
                ans += (c << (i*8));
        }
        return ans;
}

static void write_len(int x,char* w){
        for(int i = 0;i<len_size;i++){
                char c = ((x<<((3-i)*8))>>((3-i)*8));
                w[i] = c;
                x>>=(8);
        }
}

int write_to(int fd,std::string msg){
        char wbuf[4+MAX_BUF] = {};
        write_len(msg.size(),wbuf);
        memcpy(wbuf+4,msg.data(),msg.size());
        return write_all(fd, wbuf,4+msg.size());
}

int read_from(int fd,char* rbuf){
        int len = parse_len(fd);
        if(len<0){
                return -1;
        }
        assert(len<MAX_BUF);
        int n = read_all(fd, rbuf,len);
        if(n<0){
                return -1;
        }
        return 0;
}
