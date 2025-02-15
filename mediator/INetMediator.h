#pragma once
#include<QObject>
//先声明有INet这个类，让编译器直接使用,因为代码里有这个类,所以会被编译到
class INet;
class INetMediator:public QObject {
    Q_OBJECT
signals:
    //把接收到的数据发给中介者类的信号
    void sig_dealData(char* data, int len, unsigned long from);
public:
    INetMediator();
    ~INetMediator();

	//打开网络
	virtual bool openNet() = 0;

	//关闭网络
	virtual void closeNet() = 0;
	 
	//发送数据
	virtual bool sendData(char* data, int len, unsigned long to) = 0;

	//接收数据
	//Tcp:socket决定了是谁发送过来的数据
	//Udp:ip决定了是谁发送过来的数据
	virtual void recvData(char* data ,int len, unsigned long from) = 0;
protected:
	INet* m_pNet;
};
