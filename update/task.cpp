#include "task.h"

task::task(TASKTYPE type, u32 duration, u32 id_)
{
    mRevTimePoint = std::chrono::system_clock::now();
    mType = type;
    mTimeDuriation = duration;
    id = id_;
    mResult.data = 0;
    mResult.parts.exepected = 1;
}
bool task::isTimeout()
{
   auto tp = std::chrono::system_clock::now();
   u32 hasPassed= std::chrono::duration_cast<std::chrono::seconds>(tp - mRevTimePoint).count();
   return hasPassed>=mTimeDuriation;
}

resultOfTask task::getResult(){
    std::lock_guard<std::mutex> g(mMut);
    return mResult;}

void task::incFail(){
    std::lock_guard<std::mutex> g(mMut);
    mResult.parts.failed++;
    notifyUpdate(mResult.data);
}
void task::incSuc(){
    std::lock_guard<std::mutex> g(mMut);
    mResult.parts.suc++;
    notifyUpdate(mResult.data);
}
void task::setExpectedCfm(u32 num)
{
   // assert(num<100);
    std::lock_guard<std::mutex> g(mMut);
    mResult.parts.exepected = num;
}


u32  generateSerial()
{
    static u32 num = 0;
    return ++num;
}
