#include <iostream>
#include <thread>
#include <stdlib.h>
#include <vector>
#include <unistd.h>
#include "Semaphore.h"
using namespace std;

Semaphore aDone(0);
Semaphore bDone(0);
Semaphore cDone(1);
Semaphore mtx(1);

int Binstances = 0;

void threadA() {
    cDone.P();
    sleep(1);
    mtx.P();
    cout << "thread A complete" << endl;
    aDone.V();
    aDone.V();
    mtx.V();
}

void threadB() {
    aDone.P();
    sleep(2);
    mtx.P();
    cout << "thread B complete" << endl;
    if (++Binstances == 2) {
        Binstances = 0;
        bDone.V();
    }
    mtx.V();
}

void threadC() {
    bDone.P();
    sleep(3);
    mtx.P();
    cout << "thread C complete" << endl << endl;
    cDone.V();
    mtx.V();
}