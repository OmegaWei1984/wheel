#include <iostream>
#include "Wheel.h"

using namespace std;

Wheel *Wheel::inst;

Wheel::Wheel() {
    inst = this;
}

void Wheel::start() {
    cout << "hello, wheel!" << endl;
}
