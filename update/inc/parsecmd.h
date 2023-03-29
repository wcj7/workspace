
#ifndef __PARSE__CMD__H__
#define __PARSE__CMD__H__
#include <string>
#include "cJSON.h"
#include "task.h"
using namespace std;


bool parseCMD(string cmd, _orignalTask* pt)
{
    cJSON *pb = cJSON_Parse(cmd.c_str());
    if(pb == 0)
      return false;
    cJSON* ptype = cJSON_GetObjectItem(pb,"type");
    cJSON* ppath = cJSON_GetObjectItem(pb,"filepath");
    cJSON* psession = cJSON_GetObjectItem(pb,"session");
    if(ptype == 0 || ppath == 0 || psession == 0)
    {
        cJSON_Delete(pb);
        return false;
    }
    pt->type = ptype->valuestring;
    pt->path = ppath->valuestring;
    pt->sessionId = psession->valueint;
    unsigned int pos = pt->path.find_last_of('/');
    assert(pos != string::npos);
    pt->package = pt->path.substr(pos+1);
    cJSON_Delete(pb);
    pt->des =cmd;
    return true;
}

#endif