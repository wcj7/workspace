#ifndef TASKADMIN_H
#define TASKADMIN_H

#include "task.h"
#include <memory>
#include "port.h"
class taskAdmin
{
    taskAdmin();
    task* mDefer;
    portX* mPort;
    shared_ptr<_orignalTask> mt;

    public:
    void createTask(shared_ptr<_orignalTask>);
    void notifyFinish(task* p, u32 r);
    void update();
    void setPort(portX* p){mPort = p;}
    static taskAdmin* getInstance()
    {
        static taskAdmin t;
        return &t;
    }

    /*
     * check if received the duplicate command  
    */
    bool isSomeCommandInExecution(const string& cmd);

    private:
    bool parseJson(char*, shared_ptr<_orignalTask>);
    void sendReplay(int ind);
};

#endif // TASKADMIN_H
