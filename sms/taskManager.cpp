#include "taskManager.h"
#include <algorithm>
#include "log.h"
#include <functional>
static taskManager instant;
/*void taskManager::goThroughTasks(function<int(MSG *)> taskRoutine)
{
    for (auto iter = m_tasks.begin(); iter != m_tasks.end();)
    {
        if (taskRoutine((*iter).get()) <= 0) // check and send
        {
            LOG_DEBUG("INFO: erase the MsgId %d for task is done\n", (*iter)->data.msgId);
            iter = m_tasks.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}*/
void taskManager::goThroughTasks(time_t currTm, function<void(const char *, int)> sendto)
{
    for (auto iter = m_tasks.begin(); iter != m_tasks.end();)
    {
        if ((*iter)->ctr.start <= currTm) // check and send
        {
            sendto((*iter)->data.txtMsg1.c_str(), (*iter)->data.txtMsg1.length()); // just temp test, should be replaced when the protocal is ready
            (*iter)->ctr.start += (*iter)->ctr.interv * 60;
            if ((*iter)->ctr.start > (*iter)->ctr.end)
            {

                LOG_DEBUG("INFO: erase the MsgId %d for task is done\n", (*iter)->data.msgId);
                iter = m_tasks.erase(iter);
                continue;
            }
        }
        iter++;
    }
}

int taskManager::executeNewMsg(shared_ptr<MSG> msg)
{
    if (msg->ctr.start == 0)
    { // remove the specific msg in list with the same msgId
        LOG_DEBUG("INFO: remove the MsgId %d for SIEMENTS server query\n", msg->data.msgId);
        auto iter = find_if(m_tasks.begin(), m_tasks.end(), [=](shared_ptr<MSG> d) -> bool
                            { return msg->data.msgId == d->data.msgId; });
        if (iter != m_tasks.end())
        {
            m_tasks.erase(iter);
        }
        else
        {
            LOG_DEBUG("INFO: remove the MsgId %d failed for not existing\n", msg->data.msgId);
        }
    }
    else
    {
        LOG_DEBUG("INFO: add the MsgId %d to task list\n", msg->data.msgId);
        m_tasks.push_back(msg);
    }
    return 0;
}

taskManager *getTaskManager()
{
    return &instant;
}