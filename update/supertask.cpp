#include "supertask.h"
#include "subtask.h"
#include "subtaskLocal.h"
#include "taskpool.h"
#include "taskadmin.h"
#include "configs.h"
#include "log.h"
#include <map>
extern u32 generateSerial();
extern map<string, string> gScript;
 bool superTask::exec()
 {
    //move to the ftp dir
    string ftpdir = configs::getInstance()->getValue("ftp_dir");
    string localIp = configs::getInstance()->getValue("local_ip");
    shared_ptr<_orignalTask> t = get();//get the task all information

    string cmd = "cp " + t->path + " "+ ftpdir + "/";
    int ret = system(cmd.c_str());
    LOG_DEBUG("INFO: type[%s] super task preaction : %s, ret %d\n",t->type.c_str(), cmd.c_str(), ret);
    if(ret != 0)
      return false;

    cmd = "cp ./updateConfig/" + gScript[t->type] + " " + ftpdir + "/";
    ret = system(cmd.c_str());
    LOG_DEBUG("INFO:  super task preaction for default script: %s, ret %d\n", cmd.c_str(), ret);
    if(ret != 0)
      return false;

    if (t->type != "media_source")
    {
      u32 indLocal= 0xffffffff;
      for(u32 i = 0; i < t->ip.size(); i++)
      {
        if(t->ip[i] != localIp)
        {
          taskPool::getInstant()->addSubTask(new subTask(APP_UPDATE, 30, generateSerial(), i, this));
        }
        else
        {
          indLocal = i;//is set to be valid, push back in list in the end
        }
      }
      if(indLocal != 0xffffffff)
      {//execut local at last
         taskPool::getInstant()->addSubTask(new subTask(APP_UPDATE, 30, generateSerial(), indLocal, this));
      }
      setExpectedCfm(t->ip.size());
    }
    else
    {// for media_source only local updated is needed
      taskPool::getInstant()->addSubTask(new subTaskLocal(APP_UPDATE, 30, generateSerial(), 0, this));
      setExpectedCfm(1);
    }
    return true;
 }
 
 void superTask::notifyUpdate(u32 sig)
 {
     resultOfTask r;
     r.data = sig;
     if(r.parts.exepected == r.parts.failed + r.parts.suc)
     {
        //we have all the expected confim from sub task now
        if(r.parts.failed != 0)
        {// Failure happens
        }
        //Report to Admin for last process
        taskAdmin::getInstance()->notifyFinish(this, r.data);
        string cmd = "rm -f "+configs::getInstance()->getValue("ftp_dir")+ "/"+ get()->package;
        int ret = system(cmd.c_str());
        LOG_DEBUG("INFO: superTask ending operation :%s, ret %d\n",cmd.c_str(), ret);
     }
 }