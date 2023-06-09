#include <iostream>
#include <unistd.h>
#include "Worker.h"
#include "Service.h"
#include "Wheel.h"

void Worker::checkAndPutGlobal(shared_ptr<Service> srv)
{
    if (srv->isExiting) {
        return;
    }

    {
        lock_guard<mutex> lock(srv->queueMutex);
        if (!srv->msgQueue.empty()) {
            Wheel::inst->pushGQueue(srv);
        }
        else {
            srv->setInGlobal(false);
        }
    }
}

void Worker::operator() () {
    while (true) {
        shared_ptr<Service> srv = Wheel::inst->popGQueue();
        if (!srv) {
            // usleep(100);
            Wheel::inst->workerWait();
        }
        else {
            srv->processMsgs(eachNum);
            checkAndPutGlobal(srv);
        }
    }
}
