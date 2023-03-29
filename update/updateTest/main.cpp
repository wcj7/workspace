//#include <QCoreApplication>
#include <sys/socket.h>
#include <sys/select.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>
#include <memory>
#include <assert.h>

/*#include <glib.h>
static void my_log_handler(const gchar *log_domain,GLogLevelFlags log_level,
                    const gchar *message,
                    gpointer user_data)
{
    //在处理函数中我们可以把日志信息输出到文件或者网络，这里只打印到stdout看看 
    //另外对于 g_print我们也可以使用
//g_set_print_handler () 重定向到文件
    g_print("[%s]Debug : %s \n",user_data,message);
}*/

using namespace std;
int main(int argc, char *argv[])
{
   short port_(12901);
   string ip_("127.0.0.1");

   /* g_log_set_handler(NULL,G_LOG_LEVEL_DEBUG|G_LOG_FLAG_FATAL|G_LOG_FLAG_RECURSION,
                          my_log_handler,"APP_TAG");
        g_debug("This is a debug msg");
    g_message("This is a message msg.");
    g_error("This is a error msg");
    g_message("This msg will never show ");*/

   if(argc == 3)
   {// use input values
       port_= atoi(argv[2]);
       ip_ = argv[1];
   }
   cout <<ip_<<":"<<port_<<endl;

   //struct sockaddr_in sender_addr;
    struct sockaddr_in addr;
    //socklen_t addr_len;
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
   // memset(&addr, sizeof(addr), 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = inet_addr(ip_.c_str());
    int ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    assert(ret == 0);
    cout<< "bind ret" <<ret<<endl;
    
    
    bool brun;
    thread t([sockfd,&brun](){
         char buf[1000];
         brun = true;
         fd_set fs;
         FD_ZERO(&fs);
         FD_SET(sockfd, &fs);
         while(brun)
         {
          cout<<"select...."<<endl;
          int r = select(sockfd+1, &fs, 0, 0, NULL);
          cout<<"select...."  << r<<endl;
          sockaddr_in sender_addr;
          unsigned int sender_addr_size;
          int len = recvfrom(sockfd, buf, 999, 0, (struct sockaddr *)&sender_addr, &sender_addr_size);
          cout <<len<<endl;
          buf[len]=0;
          cout <<buf<<endl;
          //FD_CLR(sockfd, &fs);
         }
    });
    
    
    string str;
    while(1)
    {
       cin>> str;
       if(str == "exit")
       { 
          brun = false;
          sendto(sockfd,str.c_str(), str.size() , 0, (struct sockaddr *)&addr,  sizeof(addr));
          t.join();
          break;
       }
       else if(str == "1")
       {
          string str2 ="{\"type\":\"audio_source\",\"session\": 0,\"filepath\":\"/mnt/upgrade.zip\"}";
          cout<<str2<<endl;
          struct sockaddr_in addr2;
          int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
          addr2.sin_family = AF_INET;
          addr2.sin_port = htons(12902);
          addr2.sin_addr.s_addr = inet_addr("127.0.0.1");
          sendto(sockfd,str2.c_str(), str2.size() , 0, (struct sockaddr *)&addr2,  sizeof(addr2));
       }
	   else if(str == "2")
       {
          string str2 ="{\"type\":\"media_source\",\"session\": 2,\"filepath\":\"/mnt/upgrade.zip\"}";
          cout<<str2<<endl;
          struct sockaddr_in addr2;
          int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
          addr2.sin_family = AF_INET;
          addr2.sin_port = htons(12902);
          addr2.sin_addr.s_addr = inet_addr("127.0.0.1");
          sendto(sockfd,str2.c_str(), str2.size() , 0, (struct sockaddr *)&addr2,  sizeof(addr2));
       }
	   else if(str == "3")
       {
          string str2 ="{\"type\":\"lcd_software\",\"session\": 3,\"filepath\":\"/mnt/upgrade.zip\"}";
          cout<<str2<<endl;
          struct sockaddr_in addr2;
          int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
          addr2.sin_family = AF_INET;
          addr2.sin_port = htons(12902);
          addr2.sin_addr.s_addr = inet_addr("127.0.0.1");
          sendto(sockfd,str2.c_str(), str2.size() , 0, (struct sockaddr *)&addr2,  sizeof(addr2));
       }
	   else if(str == "4")
       {
          string str2 ="{\"type\":\"drm_software\",\"session\": 4,\"filepath\":\"/mnt/upgrade.zip\"}";
          cout<<str2<<endl;
          struct sockaddr_in addr2;
          int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
          addr2.sin_family = AF_INET;
          addr2.sin_port = htons(12902);
          addr2.sin_addr.s_addr = inet_addr("127.0.0.1");
          sendto(sockfd,str2.c_str(), str2.size() , 0, (struct sockaddr *)&addr2,  sizeof(addr2));
       }
    }

   // send_len = sendto(sockfd,buf, size, 0, (struct sockaddr *)&addr, addr_len);
   // recv_len = recvfrom(sockfd, buf, size, 0, (struct sockaddr *)&sender_addr, &sender_addr_size);
    return 0;
}
