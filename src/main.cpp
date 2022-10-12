#include "Wheel.h"

int main(void) {
    new Wheel();
    Wheel::inst->start();
    Wheel::inst->wait();
    return 0;
}
