#include"UdpMediator.h"
#include"../net/Udp.h"

UdpMediator::UdpMediator() {
	m_pNet = new Udp(this);
}

UdpMediator::~UdpMediator() {
	if (m_pNet) {
		m_pNet->unInitNet();
		delete m_pNet;
		m_pNet = nullptr;
	}
}

	//打开网络
bool UdpMediator::openNet() {

	return m_pNet->initNet();
}

	//关闭网络
void UdpMediator::closeNet() {
	m_pNet->unInitNet();
}

	//发送数据
bool UdpMediator::sendData(char* data, int len, unsigned long to) {

	return m_pNet->sendData(data, len, to);
}

	//接收数据
void UdpMediator::recvData(char* data, int len, unsigned long from) {
	//TODO:把接收到的数据传给处理类
	//测试代码 :打印接收到的数据
	cout<<"UdpMediator::recvData:"<<data <<" ,len:"<<len<<" ,ip:"<<from<< endl;
}
