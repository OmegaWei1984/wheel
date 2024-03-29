#pragma once

#include <queue>
#include <thread>
#include "Msg.hpp"
#include <mutex>

using namespace std;

class Service
{
public:
    uint32_t id;
    shared_ptr<string> type;
    bool isExiting = false;
    queue<shared_ptr<BaseMsg>> msgQueue;
    mutex queueMutex;
    bool inGlobal = false;
    mutex inGlobalFlagMutex;

    Service();
    ~Service();
    void onInit();
    void onMsg(shared_ptr<BaseMsg> msg);
    void onExit();
    void pushMsg(shared_ptr<BaseMsg> msg);
    bool processMsg();
    void processMsgs(int max);
    void setInGlobal(bool isInGlobal);

private:
    shared_ptr<BaseMsg> popMsg();
    void onServiceMsg(shared_ptr<ServiceMsg> msg);
    void onAcceptMsg(shared_ptr<SocketAcceptMsg> msg);
    void onRwMsg(shared_ptr<SocketRwMsg> msg);
    void onSocketData(int fd, const char* buff, int len);
    void onSocketWritable(int fd);
    void onSocketClose(int fd);
};
