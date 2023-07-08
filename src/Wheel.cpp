#include <iostream>
#include "Wheel.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

using namespace std;

Wheel* Wheel::inst;

Wheel::Wheel()
{
    inst = this;
}

void Wheel::startWorker()
{
    for (int i = 0; i < WORKER_NUM; ++i)
    {
        cout << "start worker thread: " << i << endl;
        Worker* worker = new Worker();
        worker->id = i;
        worker->eachNum = 2 << i;
        thread* wt = new thread(*worker);
        workers.push_back(worker);
        workerThreads.push_back(wt);
    }
}

void Wheel::start()
{
    cout << "hello, wheel!" << endl;
    startWorker();
    startSocket();
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
        unique_lock<shared_mutex> wlock(serviceRwlock);
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
        shared_lock<shared_mutex> rlock(serviceRwlock);
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
        unique_lock<shared_mutex> wlock(serviceRwlock);
        services.erase(id);
    }
}

void Wheel::send(uint32_t toId, shared_ptr<BaseMsg> msg)
{
    shared_ptr<Service> to = getService(toId);
    if (!to) {
        cout << "no such service, service id: " << toId << endl;
        return;
    }
    to->pushMsg(msg);
    bool hasPush = false;
    {
        lock_guard<mutex> lock(to->inGlobalFlagMutex);
        if (!to->inGlobal) {
            pushGQueue(to);
            to->inGlobal = true;
            hasPush = true;
        }
    }
    if (hasPush) {
        checkAndWeakUp();
    }
}

shared_ptr<Service> Wheel::popGQueue()
{
    shared_ptr<Service> srv = NULL;
    {
        const lock_guard<mutex> gQueueLock(gQueueMutex);
        if (!gQueue.empty())
        {
            srv = gQueue.front();
            gQueue.pop();
            --gQueueLen;
        }
    }
    return srv;
}

void Wheel::pushGQueue(shared_ptr<Service> srv)
{
    const lock_guard<mutex> gQueueLock(gQueueMutex);
    gQueue.push(srv);
    ++gQueueLen;
}

shared_ptr<BaseMsg> Wheel::makeMsg(uint32_t source, char* buff, int len)
{
    auto msg = make_shared<ServiceMsg>();
    msg->type = BaseMsg::TYPE::SERVICE;
    msg->source = source;
    msg->buff = shared_ptr<char>(buff);
    msg->size = len;
    return msg;
}

void Wheel::checkAndWeakUp() {
    if (sleepCount == 0) {
        return;
    }
    
    if (WORKER_NUM - sleepCount <= gQueueLen) {
        cout << "weak up" << endl;
        cv.notify_one();
    }
}

void Wheel::workerWait() {
    unique_lock<mutex> lockcv(cvMutex);
    sleepCount++;
    cv.wait(lockcv);
    sleepCount--;
}

void Wheel::startSocket() {
    socketWorker = new SocketWorker();
    socketWorker->init();
    socketThread = new thread(*socketWorker);
}

int Wheel::addConn(int fd, uint32_t id, Conn::TYPE type) {
    auto conn = make_shared<Conn>();
    conn->fd = fd;
    conn->serviceId = id;
    conn->type = type;
    {
        unique_lock<shared_mutex> wlock(connsRwlock);
        conns.emplace(fd, conn);
    }
    return fd;
}

shared_ptr<Conn> Wheel::getConn(int fd) {
    shared_ptr<Conn> conn = NULL;
    {
        shared_lock<shared_mutex> rlock(connsRwlock);
        unordered_map<uint32_t, shared_ptr<Conn>>::iterator iter = conns.find(fd);
        if (iter != conns.end()) {
            conn = iter->second;
        }
    }
    return conn;
}

bool Wheel::removeConn(int fd) {
    bool result = false;
    {
        unique_lock<shared_mutex> wlock(connsRwlock);
        result = (conns.erase(fd) == 1);
    }
    return result;
}

int Wheel::listen(uint32_t port, uint32_t serviceId) {
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd <= 0) {
        cout << "listen error, listenFd <= 0" << endl;
        return -1;
    }
    fcntl(listenFd, F_SETFL, O_NONBLOCK);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int r = bind(listenFd, (struct sockaddr*)&addr, sizeof(addr));
    if (r == -1) {
        cout << "listen error, bind fail" << endl;
        return -1;
    }
    r = ::listen(listenFd, 64);
    if (r < 0) {
        return -1;
    }
    addConn(listenFd, serviceId, Conn::TYPE::listen);
    socketWorker->addEvent(listenFd);
    return listenFd;
}

void Wheel::closeConn(uint32_t fd) {
   bool succ = removeConn(fd);
   close(fd);
   if (succ) {
       socketWorker->removeEvent(fd);
   }
}
