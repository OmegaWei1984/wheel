#include "SocketWorker.hpp"
#include<iostream>
#include<unistd.h>
#include<cassert>
#include<cstring>

void SocketWorker::init() {
    cout << "SocketWorker init" << endl;
    epollFd = epoll_create(1024);
    assert(epollFd > 0);
}

void SocketWorker::operator()() {
    while (true) {
        const int EVENT_SIZE = 64;
        struct epoll_event events[EVENT_SIZE];
        int eventCount = epoll_wait(epollFd, events, EVENT_SIZE, -1);
        for (int i = 0; i < eventCount; ++i) {
            epoll_event ev = events[i];
            onEvent(ev);
        }
    }
}

void SocketWorker::addEvent(int fd) {
    cout << "addEvent fd: " << fd << endl;
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = fd;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        cout << "addEvent epoll_ctl fail: " << strerror(errno) << endl;
    }
}

void SocketWorker::removeEvent(int fd) {
    cout << "removeEvent fd: " << endl;
    epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
}

void SocketWorker::modifyEvent(int fd, bool epollOut) {
    cout << "modifyEvent fd: " << fd << " epollOut: " << epollOut << endl;
    struct epoll_event ev;
    ev.data.fd = fd;
    if (epollOut) {
        ev.events = EPOLLIN | EPOLLET | EPOLLOUT;
    }
    else {
        ev.events = EPOLLIN | EPOLLET;
    }
    epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ev);
}

void SocketWorker::onEvent(epoll_event ev) {
    cout << "onEvent" << endl;
}
