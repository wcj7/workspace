#ifndef CURLUTILS_H
#define CURLUTILS_H

extern "C"
{
#include "curl/curl.h"
}

#include <string>

using namespace std;

class CurlUtils
{
public:
    CurlUtils();
    
    //remoteName为相对于FTP根目录的路径，包含“/”
    static bool downloadFile(string remoteFullName, string localFullName);
    //remotePath为相对于FTP根目录的路径，包含“/”
    static bool uploadFile(string localFullName, string remoteFullName);
    
    static int getFileSize(FILE *file);
    
};

#endif // CURLUTILS_H
