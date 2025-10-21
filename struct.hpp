#include <vector>
using std::vector;

#ifndef CONN
#define CONN
struct Conn{
        int fd;
        bool want_read=false;
        bool want_write=false;
        bool want_close=false;
        vector<uint8_t> write;
        vector<uint8_t> read;
};
#endif
