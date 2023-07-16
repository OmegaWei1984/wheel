#pragma once
#include <cstdint>
#include <memory>

using namespace std;

class BaseMsg
{
public:
    enum TYPE
    {
        SERVICE = 1,
        SOCKET_ACCEPT = 2,
        SOCKET_RW = 3,
    };
    uint8_t type;
    char load[999999]{};
    virtual ~BaseMsg(){};
};

class ServiceMsg : public BaseMsg 
{
public:
    uint32_t source;
    shared_ptr<char> buff;
    size_t size;
};

class SocketAcceptMsg : public BaseMsg
{
public:
    int listenFd;
    int clintFd;
};

class SocketRwMsg : public BaseMsg
{
public:
    int fd;
    bool isRead = false;
    bool isWrite = false;
};
