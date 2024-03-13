# ticket
电影票预约系统
构建系统 linux ，编译器 vscode，语言C++
1.该项目由登录/注册、查看电影票、预约、显示预约信息、取消预约五大功能模块
2.基于TCP传输协议
3.Json格式保证传输格式
4.内部封装了mylibevent库
5.添加了mysql数据库的连接

# 使用指南
1.先编译client.cpp和server.cpp文件
server.cpp命令
g++ -o server server.cpp -levent -ljsoncpp -lmysqlclient
client.cpp命令
g++ -o client client.cpp -levent -ljsoncpp -lmysqlclient
2.先开启server端，再开启client端，否则连接不成功
没有账户可先注册，再选择登录
3.界面
![image](https://github.com/muskdeyuan/ticket/assets/132809403/4b8a0d6c-c628-40f1-a487-5b739d9c4a72)
![image](https://github.com/muskdeyuan/ticket/assets/132809403/75d2c80a-ccf8-4ad6-a836-cc91828b708b)
![image](https://github.com/muskdeyuan/ticket/assets/132809403/c67f2da7-9b6f-4aeb-8851-1f414d68b15c)
![image](https://github.com/muskdeyuan/ticket/assets/132809403/b03dcaa8-8bf2-4c3b-a0ef-188b77137a5a)
![image](https://github.com/muskdeyuan/ticket/assets/132809403/3f158403-6128-4a33-9eaa-ede1f7c1439a)

