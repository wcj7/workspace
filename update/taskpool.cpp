#include "taskpool.h"
#include <algorithm>
taskPool::taskPool()
{

}
void taskPool::addSubTask(task* t)
{
    lock_guard<mutex> l(mUTask);
    subTQ.push_back(t);
}

task* taskPool::getSubTask()
{
    lock_guard<mutex> l(mUTask);
    if(subTQ.empty())
        return nullptr;
    task* t = subTQ.front();
    subTQ.pop_front();
    return t;
}
void taskPool::removeTask(u32 taskId)
{
    lock_guard<mutex> l(mUTask);
    auto iter = std::find_if(inputTQ.begin(),inputTQ.end(),[taskId](task* t)->bool{return t->getId() == taskId;});
    if(iter != inputTQ.end())
      inputTQ.erase(iter);
}
void taskPool::addTask(task* t)
{
    lock_guard<mutex> l(mUTask);
    inputTQ.push_back(t);
}

//bool taskPool::getFinishTask(u32& id)
//{
//    lock_guard<mutex> l(mUId);
//     if(completeTQ.empty())
//     {
//         return false;
//     }
//     id = completeTQ.front();
//     completeTQ.pop_front();
//     return true;
//}
//void taskPool::putInFinishTask(u32 id)
//{
//   lock_guard<mutex> l(mUId);
//   completeTQ.push_back(id);
//}
