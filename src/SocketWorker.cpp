#include "SocketWorker.hpp"
#include<iostream>
#include<unistd.h>

void SocketWorker::init() {
    cout << "SocketWorker init" << endl;
}

void SocketWorker::operator()() {
    while (true) {
        cout << "SocketWorker is working" << endl;
        usleep(1000);
    }
}
