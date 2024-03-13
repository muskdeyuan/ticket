#include "server.h"

bool Mysql_Client::Mysql_Content_Server()
{
    if (mysql_init(&mysql_con) == NULL)
    {
        return false;
    }
    //cout << ser_ip.c_str() << "  " << username.c_str() << " " << passwd.c_str() <<" "<< dbname.c_str() << " " << port << endl;
    if (mysql_real_connect(&mysql_con, ser_ip.c_str(), username.c_str(), passwd.c_str(), dbname.c_str(), port, NULL, 0) == NULL)
    {
        cout << "connect mysql err" << endl;
        return false;
    }
    return true;
}
// 注册
bool Mysql_Client::Register(string tel, string name, string passwd)
{
    if (!Mysql_Content_Server())
    {
        return false;
    }
    //"insert into user_info values ('13900000000','小王‘，’123456‘，’1‘)"
    string sql = string("insert into user_info values ('") + tel + string("','") + name + string("','") + passwd + string("','1' )");
    if (mysql_query(&mysql_con, sql.c_str()) != 0)
    {
        return false;
    }
    return true;
}
// 登录
bool Mysql_Client::Mysql_DL(string tel, string name, string passwd)
{
    if (!Mysql_Content_Server())
    {
        return false;
    }
    // 输入命令
    string sql = string("select name,passwd from user_info where tel_id=") + tel;
    if (mysql_query(&mysql_con, sql.c_str()) != 0)
    {
        return false;
    }
    MYSQL_RES *r = mysql_store_result(&mysql_con);
    if (r == NULL)
    {
        return false;
    }
    // 看看返回结果有多少行
    int num = mysql_num_rows(r);
    if (num == 0)
    {
        return false;
    }
    // 获得记录
    MYSQL_ROW row = mysql_fetch_row(r);
    // 第0个就是查到的第一个字段
    if (passwd.compare(row[1]) != 0)
    {
        return false;
    }
    name = row[0];        // row[0]第一个字段，row[1]第二个字段
    mysql_free_result(r); // 释放结果集占有的空间//在堆区申请
    return true;
}

bool Mysql_Client::Mysql_CKYY(Json::Value &val)
{
    // 连接数据库有问题
    if (!Mysql_Content_Server())
    {
        cout << "连接数据库失败" << endl;
        return false;
    }
    string sql = "select * from ticket_table";
    //cout<<"sql:"<<sql<<endl;
    if (mysql_query(&mysql_con, sql.c_str()) != 0)
    {
        cout << "sql查询失败" << endl;
        return false;
    }
    MYSQL_RES *r = mysql_store_result(&mysql_con);
    //cout<<"r:"<<r<<endl;
    if (r == NULL)
    {
        cout << "读取结果集失败" << endl;
        return false;
    }

    int num = mysql_num_rows(r);
    //cout<<"num:"<<num<<endl;
    if (num != 0)
    {
        for (int i = 0; i < num; i++)
        {
            Json::Value tmp;
            MYSQL_ROW row = mysql_fetch_row(r); // 传入结果集r
            tmp["ticket_id"] = stoi(row[0]);
            tmp["ticket_name"] = row[1];
            tmp["ticket_max"] = stoi(row[2]);
            tmp["count"] = stoi(row[3]);
            tmp["day_time"] = row[4];
            val["ticket_arr"].append(tmp);
        }
    }
    //cout<<"num over"<<endl;
    mysql_free_result(r); // 释放结果集r
    val["status"] = "ok";
    val["num"] = num;
    return true;
}

bool Mysql_Client::Mysql_YD(string tel, int tk_id)
{
    if (!Mysql_Content_Server())
    {
        cout << "数据库连接失败" << endl;
        return false;
    }
    // 先获取已经预定的票数.和总票数
    // sql语句
    string sql = string("select ticket_max,count from ticket_table where ticket_id=") + to_string(tk_id);
    // 链接句柄,在连接函数中定义
    if (mysql_query(&mysql_con, sql.c_str()) != 0)
    {
        //cout<< sql <<endl;
        cout << "sql yd err" << endl;
        return false;
    }

    MYSQL_RES *r = mysql_store_result(&mysql_con);
    if (r == NULL)
    {
        cout << "sql mysql_res err" << endl;
        return false;
    }
    // 获取结果集有多少行
    int num = mysql_num_rows(r);
    if (num == 0)
    {
        cout << "无法找到要预定的票" << endl;
        mysql_free_result(r);
        return false;
    }
    // max count,获取结果集中的一行
    MYSQL_ROW row = mysql_fetch_row(r);
    //票的最大值
    int max = atoi(row[0]);
    //订出去多少张
    int count = atoi(row[1]);

    if (count >= max)
    {
        //cout<< count<<endl;
        //cout<< max <<endl;
        cout << "票已经全部售出" << endl;
        return false;
    }
    mysql_free_result(r);//这一步忘记了

    count++;       // 预定一张
    Mysql_Begin(); // 开始事务
    // 添加事务----已被设为函数
   /* string str_begin="begin";
    if(mysql_query(&mysql_con,str_begin.c_str())!=0)
    {
        cout<<"开启事务失败"<<endl;
    }
*/
    string sql_update = string("update ticket_table set count=") + to_string(count) + string(" where ticket_id=") + to_string(tk_id);

    if (mysql_query(&mysql_con, sql_update.c_str()) != 0)
    {
        cout << "update count err" << endl;
        Mysql_RollBack();
        return false;
    }

    cout << "update count 完成" << endl;
    // 往ticket_res表存信息
    // insert into ticket_res values(0,13900000000,1,"2023-08-05 15:46:56",1)
    string sql_insert = string("insert into ticket_res values(0,") + tel + string(",") + to_string(tk_id) + string(",now(),1)");
    if (mysql_query(&mysql_con, sql_insert.c_str()) != 0)
    {
        cout << "insert ticket_res err" << endl;
        Mysql_RollBack();
        return false;
    }
    //----已被设为函数
   /*string str_commit="commit";
        if(mysql_query(&mysql_con,str_commit.c_str())!=0)
    {
        cout<<"开始事务提交失败"<<endl;

        Mysql_RollBack();
    }
    */
    Mysql_Commit(); // 提交事务
    return true;
}

bool Mysql_Client::Mysql_Xs(string tel,Json::Value& val)
{
    if (!Mysql_Content_Server())
    {
        cout << "数据库连接失败" << endl;
        return false;
    }

    //select ticket_res.yd_id,ticket_table.ticket_name,
    //ticket_table.day_time from ticket_res,ticket_table where 
    //tel_id=17389112933 and ticket_table.ticket_id=ticket_res.tk_id;
    //cout<<"1-tel:"<<tel<<endl;
    string sql=string("select ticket_res.yd_id,ticket_table.ticket_name,ticket_table.day_time from ticket_res,ticket_table where tel_id=")+tel+string(" ")+string(" and ticket_table.ticket_id=ticket_res.tk_id");
    //cout<<"2-tel:"<<tel<<endl;
    if( mysql_query(&mysql_con,sql.c_str())!=0)
    {
        cout<<"查询失败"<<endl;
        return false;
    }
    MYSQL_RES * r=mysql_store_result(&mysql_con);

    if(r==NULL)
    {
        cout<<"数据集为空"<<endl;
        return false;
    }

    int num =mysql_num_rows(r);
    val["status"]="ok";
    val["num"]=num;
    if(num==0)
    {
        cout<<"num数据为空"<<endl;
        return true;
    }

    for (int i=0;i<num;i++)
    {
        MYSQL_ROW row=mysql_fetch_row(r);
        Json::Value tmp;
        tmp["yd_id"]=row[0];
        tmp["ticket_name"]=row[1];
        tmp["day_time"]=row[2];
        val["yd_arr"].append(tmp);
    }
    mysql_free_result(r);
    return true;

}

bool Mysql_Client::Mysql_Qx(string yd_id)
{
    if (!Mysql_Content_Server())
    {
        cout << "数据库连接失败" << endl;
        return false;
    }

    //select tk_id = from ticket_res where yd_id=2
    //cout<<"yd_id:"<<yd_id<<endl;
    string sql_tkid = string("select tk_id from ticket_res where yd_id=") + yd_id;
    if ( mysql_query(&mysql_con,sql_tkid.c_str()) != 0 )
    {
        return false;
    }

    MYSQL_RES *r = mysql_store_result(&mysql_con);
    if ( r == NULL )
    {
        return false;
    }

    int num = mysql_num_rows(r);
    if ( num == 0 )
    {
        mysql_free_result(r);
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(r);
    string tk_id = row[0];

    mysql_free_result(r);

    //select count from ticket_table where tk_id=1
    //cout<<"tk_id:"<<tk_id<<endl;
    string sql_count = string("select count from ticket_table where ticket_id=") + tk_id;
    if ( mysql_query(&mysql_con,sql_count.c_str()) != 0 )
    {
        return false;
    }

    r = mysql_store_result(&mysql_con);
    if ( r == NULL )
    {
        return false;
    }

    num = mysql_num_rows(r);
    if ( num == 0 )
    {
        return false;
    }

    row = mysql_fetch_row(r);
    int count = atoi(row[0]);
    mysql_free_result(r);
    count--;

    Mysql_Begin();
    //update ticket_table set cout=2 where tk_id=1

    //cout<<"yd_id:"<<yd_id<<endl;
    string sql_setcount = string("update ticket_table set count=")+to_string(count)+string(" where ticket_id=")+tk_id;
    if ( mysql_query(&mysql_con,sql_setcount.c_str()) != 0)
    {
        Mysql_RollBack();
        return false;
    }

    //delete ticket_res where yd_id=1
    string sql_del_ydid = string("delete from ticket_res where yd_id=") + yd_id;
    if( mysql_query(&mysql_con,sql_del_ydid.c_str()) != 0 )
    {
        Mysql_RollBack();
        return false;
    }

    Mysql_Commit();

    return true;
}

// 事务的处理
void Mysql_Client::Mysql_Begin()
{
    string str_begin = "begin";
    if (mysql_query(&mysql_con, str_begin.c_str()) != 0)
    {
        cout << "开始事务失败" << endl;
    }
}

void Mysql_Client::Mysql_Commit()
{
    string str_commit = "commit";
    if (mysql_query(&mysql_con, str_commit.c_str()) != 0)
    {
        cout << "开始事务提交失败" << endl;
    }
}
void Mysql_Client::Mysql_RollBack()
{
    if (mysql_query(&mysql_con, "rollback") != 0)
    {
        cout << "事务回滚" << endl;
    }
}
void CallBack_Fun(int fd, short ev, void *arg)
{

    CallBack_Sock *ptr = (CallBack_Sock *)arg;
    if (ptr == NULL)
    {
        return;
    }

    if (ev & EV_READ)
    {
        ptr->Call_Back_Fun();
    }
}
// sockfd 有客户端连接 调用该方法
void CallBack_Accept::Call_Back_Fun()
{
    int c = accept(sockfd, NULL, NULL);
    if (c < 0)
    {
        return;
    }

    printf("accept c=%d\n", c);

    CallBack_Recv *p = new CallBack_Recv(c);
    if (p == NULL)
    {
        close(c);
        return;
    }

    plib->Event_Add(c, p);
}

void CallBack_Recv::Send_err()
{
    Json::Value res_val;
    res_val["status"] = "err";
    // cout << "send err?" << endl;
    send(c, res_val.toStyledString().c_str(), strlen(res_val.toStyledString().c_str()), 0);
}
void CallBack_Recv::Send_ok()
{
    Json::Value res_val;
    res_val["status"] = "ok";
    send(c, res_val.toStyledString().c_str(), strlen(res_val.toStyledString().c_str()), 0);
}
// 注册函数

void CallBack_Recv::Register()
{
    string tel = m_val["user_tel"].asString();
    string name = m_val["user_name"].asString();
    string passwd = m_val["user_passwd"].asString();

    cout << "tel:" << tel << endl;
    cout << "name:" << name << endl;
    cout << "passwd::" << passwd << endl;
    if (tel.empty())
    {
        Send_err();
        cout << "tel empty send err" << endl;
        return;
    }
    /*if(tel.empty()||name.empty()||passwd.empty())
    {
        Send_err();
        cout<<"empty send err"<<endl;
        return ;
    }*/
    if (name.empty())
    {
        Send_err();
        cout << "name empty send err" << endl;
        return;
    }
    if (passwd.empty())
    {
        Send_err();
        cout << "passwd empty send err" << endl;
        return;
    }
    // 存入数据库
    Mysql_Client mysqlcli;
    if (!mysqlcli.Register(tel, name, passwd))
    {
        Send_err();
        cout << "存入数据库失败" << endl;
        return;
    }
    // 连接服务器
    // bool Mysql_Content_Server();
    // 测试
    Send_ok();
}
// 登录
void CallBack_Recv::DL_User()
{
    string tel = m_val["user_tel"].asString();
    string passwd = m_val["user_passwd"].asString();
    if (tel.empty() || passwd.empty())
    {
        Send_err();
        cout << "tel or passwd is empty" << endl;
        return;
    }
    string name;
    Mysql_Client cli;
    if (!cli.Mysql_DL(tel, name, passwd))
    {
        Send_err();
    }
    Json::Value res_val;
    res_val["status"] = "ok";
    res_val["user_name"] = name;
    send(c, res_val.toStyledString().c_str(), strlen(res_val.toStyledString().c_str()), 0);
}

void CallBack_Recv::Ck_YuYue()
{
    cout<<"ckyy"<<endl;
    Json::Value res_val;
    Mysql_Client cli;
    if (!cli.Mysql_CKYY(res_val))
    {
        Send_err();
        return;
    }
    send(c, res_val.toStyledString().c_str(), strlen(res_val.toStyledString().c_str()), 0);
}
void CallBack_Recv::YY_ticket()
{
    string tel = m_val["user_tel"].asString();
    int tk_id = m_val["ticket_id"].asInt();

    Mysql_Client cli;
    if (!cli.Mysql_YD(tel, tk_id))
    {
        Send_err();
        return;
    }

    Send_ok();
    return;
}
//显示预约
void CallBack_Recv::Xs_Yuding()
{
    string tel=m_val["user_tel"].asString();
    Json::Value val;

    Mysql_Client cli;
    //cout<<"显示预约:tel:"<<tel<<"    val:"<<val<<endl;
    if(!cli.Mysql_Xs(tel,val))
    {
        cout<<"数据库已经运行完毕，且出错"<<endl;
        //cout<<"mysql_err"<<endl;
        Send_err();
        return ;
    }

    send(c,val.toStyledString().c_str(),(strlen(val.toStyledString().c_str())),0);
    //Send_ok();
    return ;
}
//取消预约
void CallBack_Recv::Qx_Yuding()
{
    string yd_id = m_val["yd_id"].asString();
    Mysql_Client cli;
    if ( !cli.Mysql_Qx(yd_id) )
    {
        Send_err();
        return;
    }

    Send_ok();
    return;
}

// c 有客户端发数据，调用该方法
void CallBack_Recv::Call_Back_Fun()
{
    char buff[256] = {0};
    int n = recv(c, buff, 255, 0); ////收数据  第2次开始
    if (n <= 0)
    {
        // 对方关闭
        event_free(ev);
        delete this;
        cout << "client close" << endl;
        return;
    }
    m_val.clear();
    // 反序列化
    Json::Reader Read;
    if (!Read.parse(buff, m_val))
    {
        cout << "json反序列化失败" << endl;
        Send_err();
        return;
    }
    string type_str = m_val["type"].asString(); // 转类型为string  //"ZC" "DL"
    // 创建迭代器
    map<string, enum CHO_TYPE>::iterator pos = m_map.find(type_str);
    if (pos == m_map.end())
    {
        Send_err();
        cout << "map err" << endl;
        cout << "pos:" << pos->first << " " << pos->second << " m_map.end:" << m_map.end()->first << " " << m_map.end()->second << endl;
        return;
    }
    enum CHO_TYPE cho = pos->second;
    // {DL=1,ZC,CKYY,YY,XSYY,QXYY,TCXT};
    switch (cho)
    {
    case DL:
        cout << "选择登录" << endl;
        DL_User();
        break;
    case ZC:
        cout << "选择注册" << endl;
        Register();
        break;
    case CKYY:
        cout << "查看预约" << endl;
        Ck_YuYue();
        break;
    case YY:
        cout << "预约" << endl;
        YY_ticket();
        break;
    case XSYY:
        cout << "显示预约" << endl;
        Xs_Yuding();
        break;
    case QXYY:
        cout << "取消预约" << endl;
        Qx_Yuding();
        break;
    default:
        cout << "无效输入" << endl;
        break;
    }

    /*


//测试
/*  printf("buff(c=%d)=%s\n",c,buff);
send(c,"ok",2,0);
*/
}
int main()
{
    Ser_Socket ser;
    int sockfd = ser.Socket_Init();
    if (sockfd == -1)
    {
        exit(1);
    }

    CallBack_Accept *p = new CallBack_Accept(sockfd);
    if (p == NULL)
    {
        exit(1);
    }

    MyLibevent *plib = new MyLibevent; // 定义libevent实例
    if (plib == NULL)
    {
        exit(1);
    }

    if (!plib->Event_Base_Init()) // 初始化
    {
        cout << "mylibevent init failed\n"
             << endl;
        exit(1);
    }

    p->Set_Libevent(plib);

    plib->Event_Add(sockfd, p); // 注册事件到libevent

    plib->Event_Base_Dispatch();

    delete plib;
    delete p;

    exit(0);
}
