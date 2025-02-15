#pragma once
#include "INet.h"

class TcpClient :public INet {
public:
	TcpClient(INetMediator* p);
	~TcpClient();
	//初始化网络
	bool initNet();

	//关闭网络
	void unInitNet();

	//发送数据
	bool sendData(char* data, int len, unsigned long to);

	//接收数据
	void recvData();

	// 接收数据的线程函数
	static unsigned __stdcall recvThread(void* lpVoid);
private:
	SOCKET m_sock;
	HANDLE m_handle;
	bool m_bRunning;
};