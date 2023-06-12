#pragma once

#include <cstdint>

using namespace std;

class Conn {
public:
    enum TYPE {
        listen = 1,
        client = 2,
    };
    
    uint8_t type;
    int fd;
    uint32_t serviceId;
};
