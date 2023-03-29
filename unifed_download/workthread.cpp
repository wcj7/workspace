#include "workthread.h"

WorkThread::WorkThread() :
    workTid(-1), isRun(false)
{

}

bool WorkThread::start()
{
    if (isRun) {
        return true;
    }

    isRun = true;
    if (pthread_create(&workTid, NULL, cWorkThread, this) < 0) {
        isRun = false;
        return false;
    }
    return true;
}

bool WorkThread::stop()
{
    if (!isRun) {
        return true;
    }

    isRun = false;
    pthread_join(workTid, NULL);
    return true;
}

void* WorkThread::cWorkThread(void* arg)
{
    WorkThread* th = (WorkThread*)arg;
    th->workThread();
    return NULL;
}
