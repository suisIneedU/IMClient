#include"TcpClientMediator.h"
#include"../net/TcpClient.h"
TcpClientMediator::TcpClientMediator() {
	m_pNet = new TcpClient(this);
}
TcpClientMediator::~TcpClientMediator() {
	if (m_pNet) {
		m_pNet->unInitNet();
		delete m_pNet;
		m_pNet = nullptr;
	}
}

	//打开网络
bool TcpClientMediator::openNet() {

	return m_pNet->initNet();
}

	//关闭网络
void TcpClientMediator::closeNet() {
	m_pNet->unInitNet();
}

	//发送数据
bool TcpClientMediator::sendData(char* data, int len, unsigned long to) {

	return m_pNet->sendData(data, len, to);
}

	//接收数据
void TcpClientMediator::recvData(char* data, int len, unsigned long from) {
    //把接收到的数据传给处理类
    Q_EMIT sig_dealData(data,len,from);
}
