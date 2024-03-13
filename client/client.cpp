#include "client.h"

void Socket_Client::show_info()
{
    if (!flg_dl)
    {
        cout << "---用户名：游客-----登录状态：未登录---" << endl;
        cout << "--1-- 登录               注册  --2--  " << endl;
        cout << "--------------------------------------" << endl;
        cin >> cho;
    }
    else
    {
        cout << "----用户名：" << usr_name <<"----"<<"登录状态：已登录"<< "----" << endl;
        cout << "--1-- 查看预约       --2-- 预约" << endl;
        cout << "--3-- 显示我的预约   --4-- 取消预约" << endl;
        cout << "--5-- 退出系统" << endl;
        cout << "------------------------------" << endl;
        cin >> cho;
        cho += 2; // 偏移，因为每个操作要有唯一的数字，前面两个已经用过了
    }
}
void Socket_Client::run()
{
    bool running = true;
    while (running)
    {
        show_info();
        // {DL=1,ZC,CKYY,YY,XSYY,QXYY,TCXT};
        switch (cho)
        {
        case DL:
            cout << "选择登录" << endl;
            DL_User();

            break;
        case ZC: // 代表2，因为是枚举类型
            cout << "选择注册" << endl;
            Register();
            break;
        case CKYY:
            cout<<"查看预约"<<endl;
            //cout<<"ckyy begin"<<endl;
            Ck_YuYue();
            //cout<<"ckyy over"<<endl;
            break;
        case YY:
            cout << "预约" << endl;
            Yy_ticket();
            break;
        case XSYY:
            cout << "显示预约" << endl;
            Xs_Yuding();
            break;
        case QXYY:
            cout << "取消预约" << endl;
            Qx_Yuding();
            break;
        case TCXT:
            cout << "已退出系统!" << endl;
            running = false;
            break;

        default:
            cout << "无效输入" << endl;
            break;
        }
    }
}

/*
Json::Value val;  //json定义alue数据
val["type"] = "DL";
val["user_tel"] = "13900000000";
val["user_passwd"] = "123456";
//发送数据
send(sockfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0);

char buff[128] = {0};
recv(sockfd,buff,127,0);//接收到json格式的字符串
Json::Reader Read;    //反序列化
Json::Value res;      //
Read.parse(buff,res); //反序列化成json对象
cout<<res["status"].asString()<<endl;//"ok"  //打印json对象的值
*/

/**************/
/*   预约    */
/**************/

void Socket_Client::Register()
{
    string tel;
    string name;
    string passwd;
    cout << "请输入手机号码" << endl;
    cin >> tel;
    cout << "请输入用户名" << endl;
    cin >> name;
    cout << "请输入密码" << endl;
    cin >> passwd;
    if (tel.empty() || name.empty() || passwd.empty())
    {
        cout << "输入不能为空" << endl;
        return;
    }
    // 填充Json对象
    Json::Value val;
    val["type"] = "ZC";
    val["user_tel"] = tel;
    val["user_name"] = name;
    val["user_passwd"] = passwd;
    send(sockfd, val.toStyledString().c_str(), strlen(val.toStyledString().c_str()), 0);
    char buff[128] = {0};
    int n = recv(sockfd, buff, 127, 0);
    if (n <= 0)
    {
        cout << "ser_close" << endl;
        return;
    }
    Json::Value res_val;
    Json::Reader Read;
    // 反序列化
    if (!Read.parse(buff, res_val))
    {
        // 反序列化失败
        cout << "注册 返回状态 反序列化失败" << endl;
        return;
    }
    string res_status = res_val["status"].asString();
    if (res_status.compare("ok") != 0) // 不等于0就是注册失败
    {
        cout << "注册失败" << endl;
        cout << res_status << endl;
        cout << buff << endl;

        return;
    }
    cout << "注册成功" << endl;

    usr_name = name;
    usr_tel = tel;
    flg_dl = true;
}

void Socket_Client::DL_User()
{
    string tel;
    string passwd;
    cout << "请输入手机号码" << endl;
    cin >> tel;
    cout << "请输入密码" << endl;
    cin >> passwd;
    if (tel.empty() || passwd.empty())
    {
        cout << "手机号码或密码为空" << endl;
        return;
    }
    Json::Value val;
    val["type"] = "DL";
    val["user_tel"] = tel;
    val["user_passwd"] = passwd;

    send(sockfd, val.toStyledString().c_str(), strlen(val.toStyledString().c_str()), 0); // 发送数据
    char buff[128] = {0};
    int n = recv(sockfd, buff, 127, 0);
    if (n <= 0)
    {
        cout << "ser close or err" << endl;
        return;
    }
    Json::Value res_val;
    Json::Reader Read;
    if (!Read.parse(buff, res_val))
    {
        cout << "解析失败" << endl;
        return;
    }
    string res_status = res_val["status"].asString();
    //cout << " status :" << res_val["status"].asString();
    if (res_status.compare("ok") != 0)
    {
        cout << "手机号或密码错误" << endl;
        return;
    }

    usr_name = res_val["user_name"].asString();
    cout<<  usr_name  <<endl;
    usr_tel = tel;
    flg_dl = true;
    cout << "登录成功" << endl;
    return;
}

void Socket_Client::Ck_YuYue()
{
    Json::Value val;
    val["type"] = "CKYY";
    send(sockfd, val.toStyledString().c_str(), strlen(val.toStyledString().c_str()), 0);
    char buff[1024] = {0};
    int n = recv(sockfd,buff,1023,0);    cout << "buff"<<endl;
    //cout<<buff<< endl;

    Json::Value res_val;
    Json::Reader Read;
    if (!Read.parse(buff, res_val))
    {
        cout << "json ckyy解析失败" << endl;
        return;
    }
    string status_str = res_val["status"].asString();
    if (status_str.compare("ok") != 0)
    {
        cout << "查看预约信息失败" << endl;
        return;
    }
    int num = res_val["num"].asInt();
    if (num <= 0)
    {
        cout << "没有可预约信息" << endl;
        return;
    }

    m_map.clear();

    cout << "| 序号 | 地点名称 | 总票数 | 已预定票数 | 时间 |" << endl;
    for (int i = 0; i < num; i++)
    {
        m_map.insert(make_pair(i, res_val["ticket_arr"][i]["ticket_id"].asInt()));

        cout << "   " << i << "  ";
        cout << "   " << res_val["ticket_arr"][i]["ticket_name"].asString() << "    ";
        cout << "   " << res_val["ticket_arr"][i]["ticket_max"].asInt() << "    ";
        cout << "   " << res_val["ticket_arr"][i]["count"].asInt() << "    ";
        cout << "   " << res_val["ticket_arr"][i]["day_time"].asString() << "  " << endl;
    }
}

void Socket_Client::Yy_ticket()
{
    cout << "请输入要预定的编号" << endl;
    int index = -1;
    cin >> index;
    map<int, int>::iterator pos = m_map.find(index);
    if (pos == m_map.end())
    {
        cout << "输入编号有误" << endl;
        return;
    }
    int tk_id = pos->second;
    Json::Value val;
    val["type"] = "YY";
    val["user_tel"] = usr_tel;
    val["ticket_id"] = tk_id;
    send(sockfd, val.toStyledString().c_str(), strlen(val.toStyledString().c_str()), 0);

    char buff[128] = {0};
        
    //cout<<"recv begin"<<endl;
    int n = recv(sockfd,buff,127,0);
    //cout<<"recv ing"<<endl;
    if (n <= 0)
    {
        cout<<"n<=0"<<endl;
        cout << "服务器关闭" << endl;
        return;
    }    
    //cout<<"recv over"<<endl;

    Json::Value res_val;
    Json::Reader Read;
    if (!Read.parse(buff, res_val))
    {
        cout << "无法解析json" << endl;
        return;
    }
    string status_str = res_val["status"].asString();
    //cout<<"status_str"<<status_str<<endl;
    if (status_str.compare("ok") != 0)
    {
        cout << "预定失败~" << endl;
        return;
    }
    cout << "预定成功!" << endl;
    return;
}

void Socket_Client::Xs_Yuding()
{
    Json::Value val;
    val["type"] = "XSYY";
    val["user_tel"] = usr_tel;
    send(sockfd, val.toStyledString().c_str(), strlen(val.toStyledString().c_str()), 0);
    char buff[1024] = {0};
    int n = recv(sockfd, buff, 1023, 0);
    if (n <= 0)
    {
        cout << "ser close" << endl;
        return;
    }

    //解析数据
    Json::Value res_val;
    Json::Reader Read;
    //存储数据
    if (!Read.parse(buff, res_val))
    {
        cout << "json解析失败" << endl;
        return;
    }
    //查看转换状态
    string status_str = res_val["status"].asString();
    cout<<"status_str:"<<  status_str  <<endl;
    if (status_str.compare("ok") != 0)
    {
        cout << "查看预约信息失败" << endl;
        return;
    }

    int num = res_val["num"].asInt();
    if (status_str.compare("ok") != 0)
    {
        cout << "暂无预定信息" << endl;
        return;
    }
    m_xs_map.clear();
    cout << "---------我的预约信息---------" << endl;
    cout << "|序号|   名称   |    时间    |" << endl;
    for (int i = 0; i < num; i++)
    {
        m_xs_map.insert(make_pair(i + 1, res_val["yd_arr"][i]["yd_id"].asString()));
        cout << " " << i + 1;
        cout << "    " << res_val["yd_arr"][i]["ticket_name"].asString();
        cout << "       " << res_val["yd_arr"][i]["day_time"].asString();
        cout << endl;
    }
    cout << "------------------------------" << endl;
}

void Socket_Client::Qx_Yuding()
{
    Xs_Yuding();
    cout << "请输入要取消的编号" << endl;
    int index = -1;
    cin >> index;
    map<int, string>::iterator pos = m_xs_map.find(index);
    if (pos == m_xs_map.end())
    {
        cout << "输入有误" << endl;
        return;
    }

    string yd_id = pos->second;
    Json::Value val;
    val["type"] = "QXYY";
    val["yd_id"] = yd_id;

    send(sockfd, val.toStyledString().c_str(), strlen(val.toStyledString().c_str()), 0);
    char buff[128] = {0};
    int n = recv(sockfd, buff, 127, 0);
    if (n <= 0)
    {
        cout << "ser close" << endl;
        return;
    }
    Json::Value res_val;
    Json::Reader Read;
    if (!Read.parse(buff, res_val))
    {
        cout << "json解析失败" << endl;
        return;
    }
    string status_str = res_val["status"].asString();
    if (status_str.compare("ok") != 0)
    {
        cout << "取消预定失败" << endl;
        return;
    }
    cout << "取消成功" << endl;
    return;
}

int main()
{
    Socket_Client client;
    if (!client.Connect_Server())
    {
        exit(1);
    }

    client.run();

    exit(0);
}
