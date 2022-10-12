#pragma once
#include <vector>
#include "Worker.h"

class Wheel
{
public:
    static Wheel *inst;

    Wheel();
    void start();
    void wait();

private:
    int WORKER_NUM = 3;
    vector<Worker *> workers;
    vector<thread *> workerThreads;

    void startWorker();
};
