#include "struct.hpp"

Conn* handle_accept(int fd);

void handle_read(Conn* con);

void handle_write(Conn* con);
