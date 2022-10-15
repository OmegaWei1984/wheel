#pragma once

#include <queue>
#include <thread>
#include "Msg.h"
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

    Service();
    ~Service();
    void onInit();
    void onMsg(shared_ptr<BaseMsg> msg);
    void onExit();
    void pushMsg(shared_ptr<BaseMsg> msg);
    bool processMsg();
    void processMsgs(int max);

private:
    shared_ptr<BaseMsg> popMsg();
};
