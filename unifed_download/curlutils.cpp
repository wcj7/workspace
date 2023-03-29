#include "curlutils.h"

#include "log.h"

CurlUtils::CurlUtils()
{
    
}

bool CurlUtils::downloadFile(string remoteFullName, string localFullName)
{
    bool ret=true;
    
    CURL *curl;
    CURLcode res;
    
    
    FILE *fp = fopen(localFullName.c_str(), "w");
    if (NULL == fp) {
        return false;
    }
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, remoteFullName.c_str());
        /* Set a pointer to our struct to pass to the callback */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        /* Switch on full protocol/debug output */
//        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        
        res = curl_easy_perform(curl);
        
        /* always cleanup */
        curl_easy_cleanup(curl);
        
        if(CURLE_OK != res) {
            /* we failed */
            ret = false;
        }
    } else {
        ret = false;
    }
    
    if (fp) {
        fclose(fp); /* close the local file */
    }
    
    curl_global_cleanup();
    
    LOG_DEBUG("Download file %s %s\n", remoteFullName.c_str(), (ret==true?" OK":" Fail"));
    return ret;
}

bool CurlUtils::uploadFile(string localFullName, string remoteFullName)
{
    bool ret=true;
    
    CURL *curl;
    CURLcode res;
    FILE *fp = fopen(localFullName.c_str(), "r");
    if (NULL == fp) {
        return false;
    }
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, remoteFullName.c_str());
        /* Set a pointer to our struct to pass to the callback */
        curl_easy_setopt(curl, CURLOPT_READDATA, fp);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE, getFileSize(fp));
        curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1);
        /* Switch on full protocol/debug output */
//        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        
        res = curl_easy_perform(curl);
        
        /* always cleanup */
        curl_easy_cleanup(curl);
        
        if(CURLE_OK != res) {
            /* we failed */
            ret = false;
        }
    } else {
        ret = false;
    }
    
    if (fp) {
        fclose(fp); /* close the local file */
    }
    
    curl_global_cleanup();
    
    LOG_DEBUG("Upload file %s %s\n", localFullName.c_str(), (ret==true?" OK":" Fail"));
    return ret;
}

int CurlUtils::getFileSize(FILE *file)
{
    int size = 0;
    fseek(file, 0L, SEEK_END);
    size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    return size;
}

