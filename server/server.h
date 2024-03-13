#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <event.h>
#include <jsoncpp/json/json.h>
#include <mysql/mysql.h>

using namespace std;
enum CHO_TYPE {DL = 1, ZC ,CKYY,YY,XSYY,QXYY,TC};
const int listen_max = 5; // 监听套接字最大事件数
class MyLibevent;

// 回调函数基类
void CallBack_Fun(int fd, short ev, void *arg);

//数据库
class Mysql_Client
{
    public:
    Mysql_Client()
    {
     ser_ip="127.0.0.1";
     port =3306;
     username="root";
     passwd="111111";//密码自定义
     dbname="prodb";//数据库名--自定义
    }
    ~Mysql_Client()
    {
        mysql_close(&mysql_con);
    }
    //注册
    bool Register(string tel,string name,string passwd);
    //登录
    bool Mysql_DL(string tel,string name,string passwd);
    
    bool Mysql_CKYY(Json::Value&val);
    bool Mysql_YD(string tel,int tk_id);
    bool Mysql_Xs(string tel,Json::Value& val);
    bool Mysql_Qx(string yd_id);

    private:
    //连接服务器
    bool Mysql_Content_Server();
    //事务的处理
    void Mysql_Begin();
    void Mysql_Commit();
    void Mysql_RollBack();
    private:
    MYSQL mysql_con;
    string ser_ip;
    short port;
    string username;
    string passwd;//密码自定义
    string dbname;//数据库名--自定义
};
// 创建套接字类
class Ser_Socket
{
public:
    // 不含参构造函数（默认）
    Ser_Socket()
    {
        m_port = 6000;     // 端口号
        ips = "127.0.0.1"; // 本主机
    }
    // 含参构造
    Ser_Socket(short port, string ips)
    {
        m_port = port;   // 端口号
        this->ips = ips; // 由于ips重名了就需要this指针来赋值
    }
    // 创建套接字函数
    int Socket_Init()
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
        {
            return -1;
        }
        struct sockaddr_in saddr;
        memset(&saddr, 0, sizeof(saddr)); // 置空
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(m_port);
        saddr.sin_addr.s_addr = inet_addr(ips.c_str());
        // 连接
        int res = bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
        if (res == -1)
        {
            cout << "bind err" << endl;
            return -1;
        }
        // 监听事件
        res = listen(sockfd, listen_max);
        if (res == -1)
        {
            return -1;
        }

        return sockfd;
    }
    // 获得套接字
    int Get_Sockfd() const
    {
        return sockfd;
    }

private:
    short m_port;
    string ips;
    int sockfd;
};

// 回调函数套接字类
class CallBack_Sock
{
public:
    virtual void Call_Back_Fun() = 0;
    struct event *ev; // 事件
};

// 回调函数接收事件类
class CallBack_Accept : public CallBack_Sock
{
public:
    // 构造函数
    CallBack_Accept(int fd) : sockfd(fd)
    {
        plib = NULL;
    }
    // 这一步是干吗
    void Call_Back_Fun();
    void Set_Libevent(MyLibevent *plib)
    {
        this->plib = plib;
    }

private:
    int sockfd;
    // 事件处理
    MyLibevent *plib;
};

class CallBack_Recv : public CallBack_Sock
{
public:
    CallBack_Recv(int fd) : c(fd)
    {
        m_map.insert( make_pair("ZC", ZC));
        m_map.insert(make_pair("DL", DL) );
        m_map.insert(make_pair("CKYY", CKYY) );
        m_map.insert(make_pair("YY", YY) );
        m_map.insert(make_pair("QXYY", QXYY) );
        m_map.insert(make_pair("XSYY", XSYY) );
    }

    void Call_Back_Fun();
    ~CallBack_Recv()
    {
        close(c);
    }
    //注册
    void Register();
    //登录
    void DL_User();
    //查看预约
    void Ck_YuYue();
    //预定票
    void YY_ticket();
    //显示预约
    void Xs_Yuding();
    //取消预约
    void Qx_Yuding();
    
    void Send_err();
    void Send_ok();

private:
    int c; // 客户端套接字

    Json::Value m_val;
    map<string, enum CHO_TYPE> m_map;
};

// libevent
class MyLibevent
{
public:
    MyLibevent()
    {
        base = NULL;
    }
    // 创建事件
    bool Event_Base_Init()
    {
        base = event_init();
        if (base == NULL)
        {
            return false;
        }
        return true;
    }
    // 添加事件
    bool Event_Add(int fd, CallBack_Sock *ptr)
    {
        struct event *ev = event_new(base, fd, EV_READ | EV_PERSIST, CallBack_Fun, ptr);
        if (ev == NULL)
        {
            return false;
        }

        if (event_add(ev, NULL) == -1)
        {
            return false;
        }

        ptr->ev = ev; // 该描述对应的struct event事件指针记录

        return true;
    }

    bool Event_Base_Dispatch()
    {
        if (base != NULL)
        {
            if (event_base_dispatch(base) == 0)
            {
                return true;
            }
        }

        return false;
    }

private:
    struct event_base *base;
};
