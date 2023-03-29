#include <iostream>
#include "executor.h"
// #include "reccommandactor.h"
#include "configs.h"
// #include <algorithm>
#include "udpPort.h"
#include <assert.h>
#include "taskadmin.h"
#include "cJSON.h"
#include "parsecmd.h"
#include "log.h"
#include <memory>
using namespace std;

/*
 *                     Update history
 * 2023-3-27   Add default upgrade script for only upgrade content upload 1.10
 *
 *
 *
 */

const string verion = "v 1.10";

int main(int argc, char *argv[])
{
  cout << verion << endl;
  if (!Log::instance().init("./log", true, 13))
  {
    cout << "Init log instance error" << endl;
    return -1;
  }

  bool b = configs::getInstance()->loadConfig("config.ini");
  if (!b)
  {
    LOG_DEBUG("Error: loadConfig file failed\n");
    return -1;
  }

  // port interact with download app
  udpPort uPort(*configs::getInstance());
  if (!uPort.initiate())
  {
    LOG_DEBUG("Error: initiate udp port failed\n");
    return -1;
  }

  taskAdmin::getInstance()->setPort(&uPort);

  u32 num = atoi(configs::getInstance()->getValue("thread_number").data());
  assert(num < 30);
  assert(1 <= num);
  LOG_DEBUG("INFO: parallel work thread number %d\n", num);
  executor taskT[num];
  for (auto &t : taskT)
  {
    t.run();
  }

  // loop of retrive the command from download app
  bool bfirstCMD = true;
  int sessionId = -1;
  // micro seconds
  u32 interval = atoi(configs::getInstance()->getValue("interval_time").data());
  while (true)
  {
    string rev = uPort.getCMD();
    if (rev != "")
    {
      if (taskAdmin::getInstance()->isSomeCommandInExecution(rev))
      { // download app will send duplicate command during the update
        LOG_DEBUG("INFO: Duplicate command is received,just ignore\n");
      }
      else
      {
        shared_ptr<_orignalTask> p1 = make_shared<_orignalTask>();
        if (parseCMD(rev, p1.get()))
        {
          // first command or  we have a bigger command session id
          if (bfirstCMD || p1->sessionId > sessionId)
          {
            sessionId = p1->sessionId;
            bfirstCMD = false;

            // process task，will be break to subtask for executor take
            taskAdmin::getInstance()->createTask(p1);
          }
          else
          {
            LOG_DEBUG("INFO: Invalid session id: current %d, cmd %d\n", sessionId, p1->sessionId);
          }
        }
        else
        {
          LOG_DEBUG("Error: parse failed, no return information\n");
        }
      }
    }

    if (rev == "") // if data is received , go on receive without rest
      this_thread::sleep_for(std::chrono::milliseconds(interval));

    // if we have task ongoing, send updating to download app, there is a request for that
    taskAdmin::getInstance()->update();
  }
  return 0;
}