#pragma once
#include <thread>
#include "Service.hpp"

class Wheel;
class Worker;

using namespace std;


class Worker {
public:
    int id;
    int eachNum;
    void operator() ();
private:
    void checkAndPutGlobal(shared_ptr<Service> srv);
};
