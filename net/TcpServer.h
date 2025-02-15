#pragma once
#include "INet.h"
#include<map>
#include<list>
class TcpServer :public INet {
public:
	TcpServer(INetMediator* p);
	~TcpServer();
	//初始化网络
	bool initNet();

	//关闭网络
	void unInitNet();

	//发送数据
	bool sendData(char* data, int len, unsigned long to);

	//接收数据
	void recvData();

	// 接收数据的线程函数
	static unsigned __stdcall acceptThread(void* lpVoid);
	static unsigned __stdcall recvThread(void* lpVoid);
private:
	SOCKET m_sock;
	bool m_bRunning;
	map<unsigned int, SOCKET> m_mapThreadIdToSocket;
	list<HANDLE> m_listHandle;
};