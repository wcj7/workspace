#include "subtask.h"
#include <thread>
#include "telnet.h"
#include "configs.h"
#include <string.h>
#include <future>
#include "log.h"
#include <map>

using namespace std;

extern map<string, string> gScript;

bool subTask::exec()
{ //check if the the ip is same as local
     if(mSuper->get()->ip[mIndex] == configs::getInstance()->getValue("local_ip"))
     {//Operate at local, run cmd directly
        return execAtLocal();
     }
     else
     {//Operate at remote, 1 telnet targetIp  2 ftpget draw file   3 run cmd
        return execAtTelnet();
     }
}
void subTask::notifyUpdate(u32 sig)
{
    resultOfTask r;
    r.data = sig;
    //upate the super task information
    r.parts.exepected == r.parts.suc? mSuper->incSuc(): mSuper->incFail();
}

bool subTask::execAtLocal()
{
    bool ret = false;
    configs* pc = configs::getInstance();

    string dir =  "mkdir -p " + pc->getValue("target_ftp_dir")+"&& ";//creat work directory in case not existing
    dir += "rm -rf " + pc->getValue("target_ftp_dir") +"/*";
    int iret = system(dir.c_str());
    LOG_DEBUG("INFO: CMD %s, ret %d\n", dir.c_str(),iret);

    u32 timeLimit = atoi(pc->getValue("max_time").c_str());
    string cmd = combineCmd(true);
    FILE* fp = popen(cmd.c_str() ,"r");
    if(fp != NULL){
        char buf[400];//there is a risk, the information could be cut off lead to fail to find the end mark
        u32 nCounter = 0;
        while(true){
            char* pt = fgets(buf, 399, fp);
            if(pt == NULL){// when we don't receive information from the commond execution exceed for a period,break the while
                this_thread::sleep_for(std::chrono::seconds(1));
                if(nCounter++> timeLimit){//check if exceed the time limitation
                    LOG_DEBUG("Warning: local task timeout %d seconds\n", timeLimit);
                    break;
                }
                continue;
            }

            if(strstr(buf, pc->getValue("update_suc_mark").c_str()) != NULL)
            {
                LOG_DEBUG("INFO: Success sub task %d\n", getId());
                ret = true;
                break;
            }
            else if(strstr(buf, pc->getValue("update_fai_mark").c_str()) != NULL){
                LOG_DEBUG("INFO: Failure sub task %d\n", getId());
                break;
            }
            nCounter = 0; //clean counter when we get information
            memset(buf, 0, 400);
        }
        pclose(fp);
    }
    return ret;
}

const int  INITIATE = 0;
const int  LINKDOWN = 1;
const int  FAILURE = 2;
const int  SUCCESS = 3;

bool subTask::execAtTelnet()
{
    bool  Done = false;
    promise<int> _result;
    telnet tnet(mSuper->get()->user, mSuper->get()->psw);
    configs* pc = configs::getInstance();
    u32 timeLimit = atoi(pc->getValue("max_time").c_str());
    
    if(false == tnet.connectServer(mSuper->get()->ip[mIndex],
        [&](string info, int err){
           if(Done){
            LOG_DEBUG("INFO: doesn't expect to parse more information after job is done\n");
            return;
           }

           if(err == -1){//socket err
              _result.set_value(LINKDOWN);
              Done = true;
           }
           else if( err == 0){//has been login, execute the commands
               string dir =  "mkdir -p " + pc->getValue("target_ftp_dir") +"&& ";//creat work directory in case not existing
               dir += "rm -rf " + pc->getValue("target_ftp_dir") +"/*"; //clean things 
               tnet.exeCMD(dir);

               string cmd = combineCmd(false);
               this_thread::sleep_for(std::chrono::microseconds(10));
               tnet.exeCMD(cmd);//send to telnet to execute
           }
           else if( err == 1){//receive information normally, check if the exepected string is received
                //there is a risk, the information could be cut off which leads to fail to find the end mark
               if(string::npos != info.find(pc->getValue("update_suc_mark"))){
                  LOG_DEBUG("INFO: Success sub task %d\n", getId());
                   _result.set_value(SUCCESS);
                   Done = true;
               }
               else if(string::npos != info.find(pc->getValue("update_fai_mark"))){
                   LOG_DEBUG("INFO: Failure sub task %d\n", getId());
                   _result.set_value(FAILURE);
                   Done = true;
               }
           }
        })) {
       return false;
    }

    auto fut = _result.get_future();
    auto status = fut.wait_for(std::chrono::seconds(timeLimit));
    if(status == future_status::timeout) {
        LOG_DEBUG("Warning: remote task timeout %d seconds\n", timeLimit);
    }
    else {
        this_thread::sleep_for(std::chrono::seconds(2));
        return fut.get() == SUCCESS;
    }
    return false;
}
/*
string subTask::combineCmd(bool blocal)
{
    configs* pc = configs::getInstance();
    //string cmd =  "mkdir -p " + pc->getValue("target_ftp_dir") + "&& ";
    string cmd = "cd "+ pc->getValue("target_ftp_dir") +"&& ";//cd to target dir
    if(blocal){// copy to current directory
      cmd += "mv " +  pc->getValue("ftp_dir");
      cmd += "/"+mSuper->get()->package + " ./ &&"; 
    }
    else{//ftpget for remote
      cmd += "ftpget -u "+ pc->getValue("ftp_user") +" -p "+ pc->getValue("ftp_password")+" ";
      cmd += configs::getInstance()->getValue("local_ip");
      cmd += " "+mSuper->get()->package + "&& ";
    }
    cmd += "unzip -o " + mSuper->get()->package + "&& ";//unzip package
    cmd += "rm -f " + pc->getValue("target_ftp_dir")+ "/"+ mSuper->get()->package + "&& ";//remove the package
    cmd += "chmod +x " + mSuper->get()->cmd + "&& "; //executable the upgrade script
    cmd += "./"+mSuper->get()->cmd; //execute upgrade script
    return cmd;
}*/
string subTask::combineCmd(bool blocal)
{
    configs* pc = configs::getInstance();
    string defaultScript = gScript[mSuper->get()->type];
    //string cmd =  "mkdir -p " + pc->getValue("target_ftp_dir") + "&& ";
    string cmd = "cd "+ pc->getValue("target_ftp_dir") +"&& ";//cd to target dir
    if(blocal){// copy to current directory
      cmd += "mv " +  pc->getValue("ftp_dir")+ "/"+mSuper->get()->package + " ./ &&"; 
      cmd += "mv " +  pc->getValue("ftp_dir")+ "/"+ defaultScript + " ./ &&"; 
    }
    else{//ftpget for remote
      cmd += "ftpget -u "+ pc->getValue("ftp_user") +" -p "+ pc->getValue("ftp_password")+" ";
      cmd += configs::getInstance()->getValue("local_ip");
      cmd += " "+mSuper->get()->package + "&& ";

      cmd += "ftpget -u "+ pc->getValue("ftp_user") +" -p "+ pc->getValue("ftp_password")+" ";
      cmd += configs::getInstance()->getValue("local_ip");
      cmd += " "+ defaultScript + "&& ";
    }
    
    //the default upgrade script
    cmd += "chmod +x " + defaultScript + "&& ";
    cmd += "./"+ defaultScript + " " + mSuper->get()->package  + " "+ mSuper->get()->cmd;
 
    //execute upgrade script
    return cmd;
}