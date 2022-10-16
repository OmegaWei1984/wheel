#pragma once

#include <unordered_map>
#include <vector>
#include <shared_mutex>

#include "Service.h"
#include "Worker.h"

class Wheel
{
public:
    static Wheel *inst;
    unordered_map<uint32_t, shared_ptr<Service>> services;
    uint32_t maxId = 0;
    shared_mutex rwlock;

    Wheel();
    void start();
    void wait();
    uint32_t newService(shared_ptr<string> type);
    void killService(uint32_t id);

private:
    int WORKER_NUM = 3;
    vector<Worker *> workers;
    vector<thread *> workerThreads;

    void startWorker();
    shared_ptr<Service> getService(uint32_t id);
};
