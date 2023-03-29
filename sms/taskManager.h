#pragma once

#include "netData.h"
#include <list>
#include <memory>
using namespace std;

// Messages received from SIEMENTS are store here till the end time exceed the current time point
class taskManager
{
    list<shared_ptr<MSG>> m_tasks; // task list rely on

public:
    /*
     * Routine run timely in main loop:
     * 1. send message to PA when the sending time is coming
     * 2. remove the message are expired( exceed the ending time)
     */
    // void goThroughTasks(function<int(MSG *)>);
    void goThroughTasks(time_t currTm, function<void(const char *, int)> sendto);
    /*
     * Perform adding an new message to task list or
     * remove the specific msg in task list(on condition ctr parts are 0)
     */
    int executeNewMsg(shared_ptr<MSG>);
};
taskManager *getTaskManager();
