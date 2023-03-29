#include "executor.h"
#include "taskpool.h"
#include <iostream>
#include "log.h"
executor::executor()
{
    mInProcess = nullptr;
    bRun = false;
}
void executor::run()
{
    bRun = true;
    std::thread t([this]()
                  {
        bool bret;
        //resultOfTask result;
        std::cout<<"exe thread start ..."<<endl;
        while(bRun)
        {
           mInProcess = taskPool::getInstant()->getSubTask();
           if(mInProcess != nullptr)
           {
              LOG_DEBUG("INFO: sub task %d starting ...\n", mInProcess->getId());
              bret = mInProcess->exec();
              bret? mInProcess->incSuc():mInProcess->incFail();
              LOG_DEBUG("INFO: sub task %d done\n", mInProcess->getId());
              /*result = mInProcess->getResult();
              std::cout<<"end sub task id:"<<mInProcess->getId()<<"  "<<result.parts.exepected<<":"<<result.parts.suc<<":"<<result.parts.failed <<endl;
              if(result.parts.exepected == result.parts.suc)
              {
                  //notify
                  std::cout<<"end task id:"<<mInProcess->getId()<<" success"<<endl;
              }
              else
              {
                  std::cout<<"end task id:"<<mInProcess->getId()<<" failure"<<endl;
              }*/
              delete mInProcess;
              mInProcess = nullptr;
           }
           std::this_thread::sleep_for(std::chrono::microseconds(300));
        }
        std::cout<<"EXE thread end"<<endl; });
    mThread.swap(t);
}
void executor::stop()
{
    bRun = false;
    mThread.join();
}
