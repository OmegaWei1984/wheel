#pragma once

#include <unordered_map>
#include <vector>
#include <shared_mutex>

#include "Service.h"
#include "Worker.h"

class Wheel
{
public:
	static Wheel* inst;
	unordered_map<uint32_t, shared_ptr<Service>> services;
	uint32_t maxId = 0;
	shared_mutex rwlock;

	Wheel();
	void start();
	void wait();
	uint32_t newService(shared_ptr<string> type);
	void killService(uint32_t id);
	void send(uint32_t toId, shared_ptr<BaseMsg>);
	shared_ptr<Service> popGQueue();
	void pushGQueue(shared_ptr<Service> srv);

private:
	int WORKER_NUM = 3;
	vector<Worker*> workers;
	vector<thread*> workerThreads;
	queue<shared_ptr<Service>> gQueue;
	int gQueueLen = 0;
	mutex gQueueMutex;

	void startWorker();
	shared_ptr<Service> getService(uint32_t id);
};
