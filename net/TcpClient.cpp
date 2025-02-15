#include"TcpClient.h"
#include"../mediator/TcpClientMediator.h"

TcpClient::TcpClient(INetMediator* p):m_sock(INVALID_SOCKET), m_handle(nullptr), m_bRunning(true){
	m_pNetMediator = p;
}
TcpClient::~TcpClient() {

}
//初始化网络(加载库、创建套接字、连接服务器、创建接受数据的线程)
bool TcpClient::initNet() {
	//1.加载库
	WORD version = MAKEWORD(2, 2);
	WSADATA data = {};
	int err = WSAStartup(version, &data);
	if (err != 0) {
		cout << "WSAStartup failed " << endl;
		return false;
	}

	if (2 != HIBYTE(data.wVersion) || 2 != LOBYTE(data.wVersion)) {
		cout << "WSAStartup version error " << endl;
		return false;
	}
	else {
		cout << "WSAStartup success " << endl;
	}

	//2.创建套接字
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sock) {
		cout << "socket error " << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "socket sucess " << endl;
	}
	//3.连接服务器
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(_DEF_TCP_PORT);
    addrServer.sin_addr.S_un.S_addr = inet_addr("47.122.50.134");
	err = connect(m_sock,(sockaddr*)&addrServer, sizeof(addrServer));
	if (SOCKET_ERROR == err){
        cout<<"connect error: " <<"TCP:"<<WSAGetLastError() << endl;
		return false;
	}
	else{
		cout<<"connect success " << endl;
	}
	
	//4.创建接受数据的线程
	m_handle = (HANDLE)_beginthreadex(NULL,0,&recvThread,this,0,NULL);
	return true;
}

//接受数据的线程函数（调用recvData）
unsigned __stdcall TcpClient::recvThread(void* lpVoid) {
	TcpClient* pThis = (TcpClient*)lpVoid;
	pThis->recvData();
	return 1;
}


//关闭网络(回收线程资源，关闭套接字，卸载库)
void TcpClient::unInitNet() {
	//1.结束线程工作(让循环自动退出)，计数器减一。
	m_bRunning = false;

	//2.关闭句柄，计数器减一。
	if (m_handle) {
		if (WAIT_TIMEOUT  ==WaitForSingleObject(m_handle, 500)) {
			TerminateThread(m_handle, -1);
		}
		CloseHandle(m_handle);
		m_handle = nullptr;
	}

	//3.关闭套接字
	if (m_sock && INVALID_SOCKET != m_sock) {
		closesocket(m_sock);
	}

	//4.卸载库
	WSACleanup();
}

//发送数据（只能发给服务端）
bool TcpClient::sendData(char* data, int len, unsigned long to) {
	//1.判断参数的合法性
	if (!data || len<=0) {
		cout<<"TcpClient::sendData paramater error" << endl;
		return false;
	}
	//2.先发包长度
	int sendNum = send(m_sock, (char*)&len, sizeof(len), 0);
	if (SOCKET_ERROR == sendNum) {
        cout<<"TcpClient::sendData send error" <<WSAGetLastError()<< endl;
		return false;
	}
	//3.再发包内容
	sendNum = send(m_sock, data, len, 0);
	if (SOCKET_ERROR == sendNum) {
        cout << "TcpClient::sendData send error" << WSAGetLastError()<< endl;
		return false;
	}
	return true;
}

//接收数据
void TcpClient::recvData() {
	//保存recv的返回值
	int recvNum = 0;
	//保存包的大小
	int packSize = 0;
	//记录一个包中累计接收到了多少数据
	int offset = 0;
	while (m_bRunning) {
		//接收包大小
		recvNum =recv(m_sock, (char*)&packSize, sizeof(packSize), 0);
		if (recvNum >0) {
			//接收包长度成功，接收包内容
			//按照包的大小申请接收包内容的空间
			char* packBuf = new char[packSize];
			//开始接收包内容
			while (packSize/*记录包中还剩余多少容量*/>0) {
				recvNum/*表示一次接收到的数据量*/ =recv(m_sock, packBuf + offset, packSize, 0);
				if (recvNum >0){
					//接收一次数据成功，累计数据量增加，剩余空间减少
						offset += recvNum;
					packSize -= recvNum;
				}
				else {
					cout<<"TcpClient::recvData recv2 error" << endl;
					break;
				}
			}
			//跳出循环，说明一个包的数据接收完毕
			//把接收到的数据发给中介者类别
			m_pNetMediator->recvData(packBuf, offset,m_sock);
			
			//offset清零
			offset = 0;
		}
		else {
			cout<<"TcpClient::recvData	reval error: " <<WSAGetLastError() << endl;
			break;
		}
	}
}
