#include "Service.h"
#include "Wheel.h"
#include <iostream>
#include <mutex>

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
}

void Service::onExit()
{
    cout << "[" << id << "] onExit" << endl;
}

void Service::onMsg(shared_ptr<BaseMsg> msg)
{
    cout << "[" << id << "] onMsg" << endl;
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
