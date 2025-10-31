#include <vector>
#include <string>
#include <algorithm>
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

#ifndef MAKE_CMD
#define MAKE_CMD
std::vector<uint8_t> make_cmd(std::string& s){
        auto strt = s.begin();
        std::cout<<s<<'\n';
        while((*strt) == ' ')strt++;
        std::vector<uint8_t> res(4,0);
        std::vector<uint8_t> len;
        while(strt<s.end()){
                auto it = std::find(strt,s.end(),(uint8_t)' ');
                len.assign(4,0);
                set_len(it-strt,len.begin());
                res.insert(res.end(),len.begin(),len.end());
                while(strt < it){
                        res.push_back(*strt);
                        strt++;
                }
                strt = it+1;
        }
        set_len(res.size()-4,res.begin());
        return res;
}
#endif
