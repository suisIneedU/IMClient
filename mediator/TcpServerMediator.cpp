#include"TcpServerMediator.h"
#include"../net/TcpServer.h"

TcpServerMediator::TcpServerMediator() {
	m_pNet = new TcpServer(this);
}
TcpServerMediator::~TcpServerMediator() {
	if (m_pNet) {
		m_pNet->unInitNet();
		delete m_pNet;
		m_pNet = nullptr;
	}
}

	//打开网络
bool TcpServerMediator::openNet() {

	return m_pNet->initNet();
}

	//关闭网络
void TcpServerMediator::closeNet() {
	m_pNet->unInitNet();
}

	//发送数据
bool TcpServerMediator::sendData(char* data, int len, unsigned long to) {

	return m_pNet->sendData(data, len, to);
}

	//接收数据
void TcpServerMediator::recvData(char* data, int len, unsigned long from) {
	//测试代码 :打印接收到的数据
	cout << "TcpServerMediator::recvData:" << data << " ,len:" << len << " ,ip:" << from << endl;

	//给客户端回复数据
	char s2[] = "我是乃龙";
	sendData(s2, sizeof(s2), from);
}
