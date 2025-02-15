#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <Winsock2.h>
#include<process.h>
#include"def.h"

#pragma comment(lib,"ws2_32.lib")

using namespace std;
//先声明有INetMediator这个类，让编译器直接使用,因为代码里有这个类,所以会被编译到
class INetMediator;
class INet {
public:
	INet():m_pNetMediator(nullptr){}
	virtual ~INet() {}


	//初始化网络
	virtual bool initNet() = 0;

	//关闭网络
	virtual void unInitNet() = 0;

	//发送数据
	virtual bool sendData(char* data, int len,unsigned long to) = 0;

	//接收数据
	virtual void recvData() = 0;

protected:
	INetMediator* m_pNetMediator;
};