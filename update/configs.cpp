#include "configs.h"
#include <regex>
#include <iostream>
#include <fstream>
#include "log.h"
#include <map>
map<string, string> gScript = {{"media_source","media.sh"},{"audio_source","audio.sh"},{"lcd_software","lcd.sh"},{"drm_software", "drm.sh"}};
configs::configs()
{
}
bool configs::loadConfig(string path)
{
   bool bret = true;
   ifstream f;
   string line;
   f.open(path,ios::in);
   if(f.is_open())
   {
     while(!f.eof())
     {
         f >>line;
         parse(line);//parse one line from the file
     }
     f.close();
   }
   else{
       bret =  false;
       LOG_DEBUG("Error: loadConfig %s failure\n", path.c_str());
   }
   return bret;
}


void configs::parse(string& line)
{
    removeSpace(line);
    match(line);
}
void configs::removeSpace(string& str)
{
    size_t index = 0;
    while( (index = str.find(' ',index)) != string::npos)
    {
        str.erase(index,1);
    }
}
bool configs::match(string& str)
{
    if(str.empty())
        return false;
    regex rx("(.+)=(.+)");
    char ar = str.front();
    if((ar ='a' &&  ar <= 'z') ||
            (ar >='A' &&  ar <= 'Z'))
    {
       std::smatch sm;
       bool br = regex_match(str, sm, rx);
       if(br && sm.size() ==3)
       {
          config_[sm[1].str()] = sm[2].str();
          return true;
       }
    }
    return false;
}
string  configs::getValue(string name)
{
      if(config_.at(name).empty())
      {
          return "";
      }
      else
      {
          return config_.at(name);
      }
}
