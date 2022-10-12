#pragma once
#include <thread>

class Wheel;
using namespace std;

class Worker {
public:
    int id;
    int eachNum;
    void operator() ();
};
