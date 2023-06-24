#include "Wheel.hpp"
#include <iostream>
#include<cassert>

int test()
{
    int id = Wheel::inst->addConn(1, 1, Conn::TYPE::listen);
    auto conn = Wheel::inst->getConn(1);
    assert(conn->fd == 1);
    bool result = Wheel::inst->removeConn(1);
    auto conn2 = Wheel::inst->getConn(1);
    assert(conn2 == NULL);
    
    auto pingType = make_shared<string>("ping");
    uint32_t ping1 = Wheel::inst->newService(pingType);
    uint32_t ping2 = Wheel::inst->newService(pingType);
    uint32_t pong = Wheel::inst->newService(pingType);

    auto msg1 = Wheel::inst->makeMsg(
        ping1, new char[3] {'h', 'i', '\0'}, 3);
    auto msg2 = Wheel::inst->makeMsg(
        ping2, new char[6] {'h', 'e', 'l', 'l', 'o', '\0'}, 6);
    Wheel::inst->send(pong, msg1);
    Wheel::inst->send(pong, msg2);

    return 0;
}

int main(void) {
    new Wheel();
    Wheel::inst->start();
    test();
    Wheel::inst->wait();
    return 0;
}
