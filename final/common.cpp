#include <vector>
#include <string>
#ifndef CONN
#define CONN
struct Conn {
        int fd;
        int port;
        bool want_read = false;
        bool want_write = false;
        bool want_close = false;
        std::string ip;
        std::vector<uint8_t> reader;
        std::vector<uint8_t> writer;
};
#endif

#ifndef SLEN
#define SLEN
void set_len(int x,std::vector<uint8_t>::iterator s){
        for(int i = 0;i<4;i++){
                char c = ((x<<(3*8))>>(3*8));
                *s = c;
                s++;
                x>>=8;
        }
}
#endif

#ifndef GLEN
#define GLEN
int get_len(std::vector<uint8_t>::iterator s){
        int ans = 0;
        for(int i = 0;i<4;i++){
                ans+=(*s<<(i*8));
                s++;
        }
        return ans;
}
#endif
