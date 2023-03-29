#ifndef TASK_H
#define TASK_H
#include <chrono>
#include "common.h"
#include <mutex>
#include <string>
#include <vector>
#include <memory>
using namespace std;
enum TASKTYPE
{
   APP_UPDATE,
   FILE_UPDATE
};
union resultOfTask
{
  u32 data;
  struct
  {
      unsigned exepected:8;
      unsigned suc:8;
      unsigned failed:8;
      unsigned :8;
  }parts;
};

struct _orignalTask
{
    /* data */
    string type;    // audio_source/media_source/lcd_software
    string path;    //absorlute path of package
    string des;      // whole command
    string package; // upgrade zip package
    string user; //telnet user name
    string psw;   //telnet password
    string cmd;   //upgrade command need to execute in zip 
    int  sessionId;
    vector<string> ip;
};

class task
{
    shared_ptr<_orignalTask> mPTask;
public:
    explicit task(TASKTYPE type, u32 duration, u32 id_);
	virtual ~task(){}
    virtual bool exec() = 0;
    virtual void notifyUpdate(u32 sig){};

	
    resultOfTask getResult();
    void incFail();
    void incSuc();
    void setExpectedCfm(u32 num);
    bool isTimeout();


    u32 getId(){return id;}
    u32 getDuriation(){return mTimeDuriation;}
    TASKTYPE getType(){return mType;}

    void set(shared_ptr<_orignalTask> pt){mPTask = pt;}
    shared_ptr<_orignalTask> get(){return mPTask;}
private:
    //Receive time
    std::chrono::system_clock::time_point mRevTimePoint;
    //Exepected to finish in this time span, minimum is 180s
    u32 mTimeDuriation;
    u32 id;
    TASKTYPE mType;

    resultOfTask mResult;
    std::mutex mMut;
};
#endif // TASK_H
