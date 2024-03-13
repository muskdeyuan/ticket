#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <jsoncpp/json/json.h>
#include<map>
using namespace std;
enum CHO_TYPE {DL=1,ZC,CKYY,YY,XSYY,QXYY,TCXT};//枚举：选择类型

//客户端类
class Socket_Client
{
public:
    Socket_Client()
    {
        ips = "127.0.0.1";
        m_port = 6000;
        flg_dl=false;
        cho=-1;
    }

    Socket_Client(string ip, short port)
    {
        ips = ip;
        m_port = port;
    }
    //容易失败的不要放在构造函数里
    //连接服务器
    bool Connect_Server()
    {
        sockfd = socket(AF_INET,SOCK_STREAM,0);
        if ( sockfd == -1 )
        {
            return false;
        }
//这个sin.这些函数都不太知道是啥意思
        struct sockaddr_in saddr;
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(m_port);
        saddr.sin_addr.s_addr = inet_addr(ips.c_str());
//saddr相关的
        if ( connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr)) == -1)
        {
            cout<<"connect err"<<endl;
            return false;
        }

        return true;
    }

    void run();
    void show_info();

    void Register();
    void DL_User();
    //系统功能
    void Ck_YuYue();
    void Yy_ticket();
    void Xs_Yuding();
    void Qx_Yuding();

private:
//登录标识
    bool flg_dl;
    string usr_name;
    string usr_tel;
    int cho;//用户选择操作

    map<int,int> m_map;
    map<int ,string> m_xs_map;

    string ips;
    short m_port;
    int sockfd;
};
