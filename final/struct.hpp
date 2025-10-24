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
