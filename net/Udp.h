#pragma once
#include "INet.h"

class Udp :public INet {
public:
	Udp(INetMediator* p);
	~Udp();
	//初始化网络
	bool initNet();

	//关闭网络
	void unInitNet();

	//发送数据
	bool sendData(char* data, int len, unsigned long to);

	//接收数据
	void recvData();

	// 接收数据的线程函数
	//beginthreadex使用recvThread函数的时候还没有创建对象，所以recvThread需要不依赖对象使用，就定义成静态的
	static unsigned __stdcall recvThread(void* lpVoid);
private:
	SOCKET m_sock;
	HANDLE m_handle;
	bool m_bRunning;
};