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

uint32_t Wheel::newService(shared_ptr<string> type)
{
    auto srv = make_shared<Service>();
    srv->type = type;
    {
        unique_lock<shared_mutex> wlock(rwlock);
        srv->id = maxId;
        ++maxId;
        services.emplace(srv->id, srv);
    }
    srv->onInit();
    return srv->id;
}

shared_ptr<Service> Wheel::getService(uint32_t id)
{
    shared_ptr<Service> srv = NULL;
    {
        shared_lock<shared_mutex> rlock(rwlock);
        unordered_map<uint32_t, shared_ptr<Service>>::
            iterator iter = services.find(id);
        if (iter != services.end())
        {
            srv = iter->second;
        }
    }
    return srv;
}

void Wheel::killService(uint32_t id)
{
    shared_ptr<Service> srv = getService(id);
    if (!srv)
    {
        return;
    }
    srv->onExit();
    srv->isExiting = true;
    {
        unique_lock<shared_mutex> wlock(rwlock);
        services.erase(id);
    }
}
