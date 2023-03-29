#ifndef SUPERTASK_H
#define SUPERTASK_H
#include "task.h"
#include "common.h"
using namespace std;


class superTask:public task
{

    std::string mCmd;
public:
    superTask(TASKTYPE type, u32 duration, u32 id):task(type,duration,id)
    {}
    //This exec is mainly to break down the task to smaller task 
    bool exec() override;
    void notifyUpdate(u32 sig) override;
};

#endif // SUPERTASK_H
