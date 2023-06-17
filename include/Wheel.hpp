#pragma once

#include <unordered_map>
#include <vector>
#include <shared_mutex>

#include "Service.hpp"
#include "Worker.hpp"
#include "SocketWorker.hpp"
#include "Conn.hpp"

class Worker;

class Wheel
{
public:
    static Wheel* inst;
    unordered_map<uint32_t, shared_ptr<Service>> services;
    uint32_t maxId = 0;
    shared_mutex serviceRwlock;
    shared_mutex connsRwlock;
    condition_variable cv;
    mutex cvMutex;
    int sleepCount = 0;

    Wheel();
    void start();
    void wait();
    uint32_t newService(shared_ptr<string> type);
    void killService(uint32_t id);
    void send(uint32_t toId, shared_ptr<BaseMsg>);
    shared_ptr<Service> popGQueue();
    void pushGQueue(shared_ptr<Service> srv);
    shared_ptr<BaseMsg> makeMsg(uint32_t source, char* buff, int len);
    void checkAndWeakUp();
    void workerWait();
    int addConn(int fd, uint32_t id, Conn::TYPE type);
    shared_ptr<Conn> getConn(int fd);
    bool removeConn(int fd);

private:
    int WORKER_NUM = 3;
    vector<Worker*> workers;
    vector<thread*> workerThreads;
    queue<shared_ptr<Service>> gQueue;
    int gQueueLen = 0;
    mutex gQueueMutex;
    SocketWorker *socketWorker;
    thread *socketThread;
    unordered_map<uint32_t, shared_ptr<Conn>> conns;

    void startWorker();
    void startSocket();
    shared_ptr<Service> getService(uint32_t id);
};
