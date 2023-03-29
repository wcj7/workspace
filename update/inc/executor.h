#ifndef EXECUTOR_H
#define EXECUTOR_H
#include <thread>
#include "taskpool.h"
class executor
{
    task* mInProcess;
    std::thread mThread;
    bool bRun;
public:
    executor();
    void run();
    void stop();
};

#endif // EXECUTOR_H
