#include "Service.hpp"
#include "Wheel.hpp"
#include <iostream>
#include <mutex>
#include <unistd.h>
#include <cstring>

Service::Service()
{
}

Service::~Service()
{
}

void Service::pushMsg(shared_ptr<BaseMsg> msg)
{
    const lock_guard<mutex> lock(queueMutex);
    msgQueue.push(msg);
}

shared_ptr<BaseMsg> Service::popMsg()
{
    shared_ptr<BaseMsg> msg = NULL;
    {
        const lock_guard<mutex> lock(queueMutex);
        if (!msgQueue.empty())
        {
            msg = msgQueue.front();
            msgQueue.pop();
        }
    }
    return msg;
}

void Service::onInit()
{
    cout << "[" << id << "] onInit" << endl;
    Wheel::inst->listen(8002, id);
}

void Service::onExit()
{
    cout << "[" << id << "] onExit" << endl;
}

void Service::onMsg(shared_ptr<BaseMsg> msg)
{
    if (msg->type == BaseMsg::TYPE::SERVICE) {
        auto m = dynamic_pointer_cast<ServiceMsg>(msg);
        onServiceMsg(m);
    } else if (msg->type == BaseMsg::TYPE::SOCKET_ACCEPT) {
        auto m = dynamic_pointer_cast<SocketAcceptMsg>(msg);
        onAcceptMsg(m);
    } else if (msg->type == BaseMsg::TYPE::SOCKET_RW) {
        auto m = dynamic_pointer_cast<SocketRwMsg>(msg);
        onRwMsg(m);
    }
}

bool Service::processMsg()
{
    shared_ptr<BaseMsg> msg = popMsg();
    if (msg)
    {
        onMsg(msg);
        return true;
    }
    else
    {
        return false;
    }
}

void Service::processMsgs(int max)
{
    for (int i = 0; i < max; ++i)
    {
        bool isSucc = processMsg();
        if (!isSucc)
        {
            break;
        }
    }
}

void Service::setInGlobal(bool isInGlobal)
{
    lock_guard<mutex> lock(inGlobalFlagMutex);
    inGlobal = isInGlobal;
}

void Service::onServiceMsg(shared_ptr<ServiceMsg> msg) {
    cout << "onServiceMsg" << endl;
}

void Service::onAcceptMsg(shared_ptr<SocketAcceptMsg> msg) {
    cout << "onAcceptMsg " << msg->clintFd << endl;
}

void Service::onRwMsg(shared_ptr<SocketRwMsg> msg) {
    int fd = msg->fd;
    if (msg->isRead) {
        const int BUFFSIZE = 512;
        char buff[BUFFSIZE];
        int len = 0;
        do {
            len = read(fd, &buff, BUFFSIZE);
            if (len > 0) {
                onSocketData(fd, buff, len);
            }
        } while (len == BUFFSIZE);

        if (len <= 0 && errno != EAGAIN) {
            if (Wheel::inst->getConn(fd)) {
                onSocketClose(fd);
                Wheel::inst->closeConn(fd);
            }
        }
    }

    if (msg->isWrite) {
        if (Wheel::inst->getConn(fd)) {
            onSocketWritable(fd);
        }
    }
}
