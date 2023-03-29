#ifndef CONFIGS_H
#define CONFIGS_H
#include <map>
#include <string>
#include <common.h>
using namespace std;
class configs
{
    map<string, string> config_;
    configs();

    void parse(string&);
    void removeSpace(string&);
    bool match(string&);
public:
   static configs* getInstance()
   {
       static configs d;
       return &d;
   }

   bool loadConfig(string path);
   string getValue(string name);
};

#define GVALUE(X) configs::getInstance()->getValue(#X)
#define GVALUETOI(X) stoi(configs::getInstance()->getValue(#X))

#endif // CONFIGS_H
