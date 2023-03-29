#ifndef __PORT__H___
#define __PORT__H___
#include "common.h"
#include <string>
using namespace std;
class portX
{
	public:
	virtual bool  initiate(string info , u32 data ) = 0;
	virtual string getCMD() = 0;
	virtual void sendMsg(string msg) = 0;
	virtual void reset() = 0;
};
#endif