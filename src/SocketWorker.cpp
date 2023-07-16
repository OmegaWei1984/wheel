#include "SocketWorker.hpp"
#include "Wheel.hpp"
#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

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
    cout << "removeEvent fd: " << fd << endl;
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
    int fd = ev.data.fd;
    auto conn = Wheel::inst->getConn(fd);
    if (conn == NULL) {
        cout << "onEvent error, can not get conn by fd(" << fd << ")" << endl;
        return;
    }
    bool isRead = ev.events & EPOLLIN;
    bool isWrite = ev.events & EPOLLOUT;
    bool isError = ev.events & EPOLLERR;
    if (conn->type == Conn::TYPE::LISTEN) {
        if (isRead) {
            onAccept(conn);
        }
    } else {
        if (isRead || isWrite) {
            onRw(conn, isRead, isWrite);
        }
        if (isError) {
            cout << "onEven error, fd is " << conn->fd << endl;
        }
    }
}

void SocketWorker::onAccept(shared_ptr<Conn> conn) {
    cout << "onAccept fd: " << conn->fd << endl;
    int clientFd = accept(conn->fd, NULL, NULL);
    if (clientFd < 0) {
        cout << "accept error" << endl;
    }
    fcntl(clientFd, F_SETFL, O_NONBLOCK);
    Wheel::inst->addConn(clientFd, conn->serviceId, Conn::TYPE::CLIENT);
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = clientFd;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
        cout << "onAccept epoll_ctl fail: " << strerror(errno) << endl;
    }
    auto msg = make_shared<SocketAcceptMsg>();
    msg->type = BaseMsg::TYPE::SOCKET_ACCEPT;
    msg->listenFd = conn->fd;
    msg->clintFd = clientFd;
    Wheel::inst->send(conn->serviceId, msg);
}

void SocketWorker::onRw(shared_ptr<Conn> conn, bool r, bool w)
{
    cout << "onRw fd:" << conn->fd << endl;
    auto msg = make_shared<SocketRwMsg>();
    msg->type = BaseMsg::TYPE::SOCKET_RW;
    msg->fd = conn->fd;
    msg->isRead = r;
    msg->isWrite = w;
    Wheel::inst->send(conn->serviceId, msg);
}