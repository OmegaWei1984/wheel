#include "Wheel.h"

int test()
{
    auto pingType = make_shared<string>("ping");
    uint32_t ping1 = Wheel::inst->newService(pingType);
    uint32_t ping2 = Wheel::inst->newService(pingType);
    uint32_t pong = Wheel::inst->newService(pingType);
    return 0;
}

int main(void) {
    new Wheel();
    Wheel::inst->start();
    test();
    Wheel::inst->wait();
    return 0;
}
