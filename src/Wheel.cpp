#include <iostream>
#include "Wheel.h"

using namespace std;

Wheel *Wheel::inst;

Wheel::Wheel()
{
    inst = this;
}

void Wheel::startWorker()
{
    for (int i = 0; i < WORKER_NUM; ++i)
    {
        cout << "start worker thread: " << i << endl;
        Worker *worker = new Worker();
        worker->id = i;
        worker->eachNum = 2 << i;
        thread *wt = new thread(*worker);
        workers.push_back(worker);
        workerThreads.push_back(wt);
    }
}

void Wheel::start()
{
    cout << "hello, wheel!" << endl;
    startWorker();
}

void Wheel::wait()
{
    if (workerThreads[0])
    {
        workerThreads[0]->join();
    }
}
