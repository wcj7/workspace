#include "taskadmin.h"
#include "taskpool.h"
#include "configs.h"
#include <memory>
#include <fstream>
#include "cJSON.h"
#include "supertask.h"
#include <string>
#include "log.h"
taskAdmin::taskAdmin()
{
  mDefer = nullptr;
}
void taskAdmin::notifyFinish(task* p, u32 r)
{
    u32 tid = p->getId();
    resultOfTask d;
    d.data = r;
    LOG_DEBUG("INFO: Supper task is done: %d:%d:%d\n", d.parts.exepected,d.parts.suc,d.parts.failed);
    taskPool::getInstant()->removeTask(tid);
    sendReplay(d.parts.failed == 0? UpdateSuc:UpdateFai);

   //Here is a little triky, can't delete p in itself notification, defer delete it
    delete mDefer;
    mDefer = p;
}
extern u32 generateSerial();
void taskAdmin::createTask(shared_ptr<_orignalTask> tp)
{
    u32 errCode = 0;
    if(taskPool::getInstant()->getTaskNumInQueue()>= 1)
    // atoi(configs::getInstance()->getValue("task_number_parallel").c_str()))
    {// queue task quannity is too high
        LOG_DEBUG("Error: Reject for there is task ongoing\n");
        errCode = 1;
    }
    else
    {
       string fileconfig = "updateConfig/";
       fileconfig += configs::getInstance()->getValue(tp->type);

       mt = tp;
       if(parseJson((char*)fileconfig.c_str(), tp))
       {
          superTask* p =  new superTask(APP_UPDATE, 1000, generateSerial());
          p->set(tp);
          LOG_DEBUG("INFO: super task will execute %d, sessionId %d\n", p->getId(), tp->sessionId);
          taskPool::getInstant()->addTask(p);
          if(!p->exec())
          {
            LOG_DEBUG("Error: super task  execute failed\n");
            taskPool::getInstant()->removeTask(p->getId());
            delete p;
            errCode = 3;
          }
       }
       else
       {
          errCode = 2;
          LOG_DEBUG("Error: parse json file failure\n");
       }
    }
    
    if(errCode != 0)
    {
       LOG_DEBUG("Warning: information the download app\n");
       sendReplay(UpdateFai);
    }
}
void taskAdmin::update()
{
    if(taskPool::getInstant()->getTaskNumInQueue()> 0)
    {
       sendReplay(Updating);
    }
}

bool taskAdmin::parseJson(char* file, shared_ptr<_orignalTask> pt)
{
   ifstream f;
   string line;
   string js;
   f.open(file,ios::in);
   if(f.is_open())
   {
     while(!f.eof())
     {
         f >>line;
         js += line;
     }
     f.close();
   }
   else{
       LOG_DEBUG("Error: open json file failure : %s\n", file);
       return false;
   }
   cJSON *pb = cJSON_Parse(js.c_str());
    if(pb == 0)
    {
      LOG_DEBUG("Error: parse json failure\n");
      return false;
    }
    cJSON* puser = cJSON_GetObjectItem(pb,"User");
    cJSON* ppassword = cJSON_GetObjectItem(pb,"Password");
    cJSON* pcmd = cJSON_GetObjectItem(pb,"CMD");
    cJSON* pip = cJSON_GetObjectItem(pb,"IP");
    if(puser == 0 || ppassword == 0 || pcmd == 0 || pip == 0)
    {
        cJSON_Delete(pb);
        LOG_DEBUG("Error: parameter is mismatch after parse\n");
        return false;
    }
    pt->cmd = pcmd->valuestring;
    pt->user = puser->valuestring;
    pt->psw = ppassword->valuestring;
    int num =  cJSON_GetArraySize(pip); //this num could be 0 when media_source
    for(int i = 0 ;i < num;i++)
    {
        cJSON* iter = cJSON_GetArrayItem(pip, i);
        pt->ip.push_back(iter->valuestring);
    }
   // cJSON_Delete(pb);
    return true;
}
void taskAdmin::sendReplay(int ind)
{
     cJSON *pb = cJSON_Parse(mt->des.c_str());
     cJSON_DeleteItemFromObject(pb, "filepath");
     cJSON* jstat =  cJSON_CreateNumber(ind);
     cJSON_AddItemToObject(pb, "status", jstat);
     char* pbuf = cJSON_PrintUnformatted(pb);
     mPort->sendMsg(pbuf);
     cJSON_Delete(pb);
     free(pbuf);
}

bool taskAdmin::isSomeCommandInExecution(const string& cmd)
{
    if(taskPool::getInstant()->getTaskNumInQueue()>= 1)
    {
        if(cmd == mt->des)
        {
            return true;
        }
    }
    return false;
}