#ifndef SUBTASK_LOCAL_H
#define SUBTASK_LOCAL_H
#include "subtask.h"
class subTaskLocal:public subTask
{
    public:
    subTaskLocal(TASKTYPE type, u32 duration, u32 id, u32 index, task* pt = nullptr):
     subTask(type,duration, id, index, pt){}

     bool exec() override{
        return execAtLocal();}
};
#endif