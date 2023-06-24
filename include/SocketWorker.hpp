#pragma once

#include<iostream>

using namespace std;

#include <sys/epoll.h>
#include <memory>
#include "Conn.hpp"

class SocketWorker {
public:
    void init();
    void operator()();
    void addEvent(int fd);
    void removeEvent(int fd);
    void modifyEvent(int fd, bool epollOut);
private:
    int epollFd;
    void onEvent(epoll_event ev);
    void onAccept(shared_ptr<Conn> conn);
    void onRw(shared_ptr<Conn> conn, bool r, bool w);
};
