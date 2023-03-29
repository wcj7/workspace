#include <tinyxml.h>
#include <iostream>
#include "xmlServer.h"
#include "log.h"
#include "configs.h"
#include "netServer.h"
#include "netData.h"
#include "taskManager.h"
#include <chrono>
#include <thread>
using namespace std;

string gTrainNo = "";
int main(int argc, char *argv[])
{
  initiateStaticLocal();
  bool b = configs::getInstance()->loadConfig("./config/profile.ini");
  if (!b)
  {
    cout << "Error: loadConfig file failed" << endl;
    return -1;
  }

  if (!Log::instance().init("./log", true, GVALUETOI(days_log)))
  {
    cout << "Init log instance error" << endl;
    return -1;
  }

  netService netSer;
  udpData fwhandle(&netSer);
  if (!fwhandle.startServerPoint(GVALUE(bind_ip), GVALUETOI(bind_port)))
  {
    LOG_DEBUG("Error: Bind port %d failure \n", GVALUETOI(bind_port));
  }

  tcpData datahandle(&netSer);
  datahandle.connectServer(GVALUE(server_IP), GVALUETOI(server_port));
  //  datahandle.sendTrainInfo("3");
  while (true)
  {
    // reconnect the tcp server in case disconnected
    if (datahandle.getState() < CONNECTED)
    {
      datahandle.tryReconnct();
    }

    // send server the train Id  when it dosen't comfirm from SIEMENTS
    if (datahandle.getState() == CONNECTED && fwhandle.getTrainNo() != "")
    { // send  train NO
      datahandle.sendTrainInfo(fwhandle.getTrainNo());
    }

    // handle coming data from net
    netSer.handle();

    // handle tasks
    time_t cur;
    time(&cur);
    /*getTaskManager()->goThroughTasks(
        [&](MSG *pmsg) -> int { // pmsg will be removed in TaskManager when the return value(ret) is equal or smaller than 0
          int ret = 1;
          if (pmsg->ctr.start <= cur)
          { // send pa when the time arrive
            fwhandle.sendData(pmsg->data.txtMsg1.c_str(), pmsg->data.txtMsg1.length(), GVALUE(peer_ip), GVALUETOI(peer_port));
            pmsg->ctr.start += pmsg->ctr.interv * 60;       // next occur time
            ret = pmsg->ctr.start >= pmsg->ctr.end ? 0 : 1; // check if the message is expired
          }
          return ret;
        });*/
    getTaskManager()->goThroughTasks(cur, bind(&udpData::sendData, &fwhandle, placeholders::_1, placeholders::_2, GVALUE(peer_ip), GVALUETOI(peer_port)));
    this_thread::sleep_for(std::chrono::milliseconds(GVALUETOI(loop_interval)));
  }
  LOG_DEBUG("INFO: App exit gently!!!\n");
  return 0;
}
