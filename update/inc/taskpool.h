#ifndef TASKPOOL_H
#define TASKPOOL_H
#include "task.h"
#include<list>
#include<mutex>
using namespace std;
class taskPool
{
   list<task*> inputTQ;   // Tasks
   list<task*> subTQ;     //SubTasks are broken down from task
 //  list<u32>   completeTQ;//task has been finished

   mutex mUTask;
//   mutex mUId;

   taskPool();
public:
   void addSubTask(task*);
   task* getSubTask();
   void removeTask(u32 taskId);
   void addTask(task*);
   u32  getTaskNumInQueue(){return inputTQ.size();}

//   bool getFinishTask(u32& id);
//   void putInFinishTask(u32 id);

   inline bool isTaskArePending(){ return !subTQ.empty();}

   static taskPool* getInstant()
   {
       static taskPool st;
       return &st;
   }
};

#endif // TASKPOOL_H
