#pragma once
#ifndef WORKTHREAD_H
#define WORKTHREAD_H

#include <string>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>

using namespace std;

class WorkThread
{
public:
    WorkThread();

    bool start();
    bool stop();
protected:
    virtual void workThread() = 0;
    bool isRun;

private:
    static void* cWorkThread(void* arg);

    pthread_t workTid;

};

#endif // WORKTHREAD_H
