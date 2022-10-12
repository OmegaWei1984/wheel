#include <iostream>
#include <unistd.h>
#include "Worker.h"

void Worker::operator() () {
    while (true) {
        cout << "worker id:" << id << endl;
        usleep(100000);
    }
}
