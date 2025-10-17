#include <cassert>
#include <unistd.h>

const int len_size = 4;

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

        int n = read_all(fd,len_buf,len_size);
        if(n<0){
                return -1;
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

