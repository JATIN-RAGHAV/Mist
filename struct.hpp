#include <netinet/in.h>
#include <vector>
using std::vector;

#ifndef CONN
#define CONN
struct Conn{
        int fd;
        bool want_read=false;
        bool want_write=false;
        bool want_close=false;
        char* addr;
        int port;
        vector<uint8_t> write;
        vector<uint8_t> read;
};
#endif
// struct sockaddr_in
// inet_ntop(AF_INET,sturct ,char*,len)
