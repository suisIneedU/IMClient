#include "Udp.h"
#include"../mediator/UdpMediator.h"

Udp::Udp(INetMediator* p):m_sock(INVALID_SOCKET),m_handle(nullptr),m_bRunning(true) {
	m_pNetMediator = p;
}

Udp::~Udp() {
	
}

	//初始化网络(加载库、创建套接字、绑定ip和端口号、创建接收数据的线程)
bool Udp::initNet() {

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
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_sock) {
		cout << "socket error " << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "socket sucess " << endl;
	}

	//3、绑定ip和端口号
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	// 魔鬼数字:把数字定义成宏
	addr.sin_port = htons(_DEF_UDP_PORT);    // 转换成网络字节序(大端)
	// 绑定所有网卡
	addr.sin_addr.S_un.S_addr = ADDR_ANY;
	err = bind(m_sock, (sockaddr*)&addr, sizeof(addr));
	if (SOCKET_ERROR == err) {
		cout << "bind error:" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "bind success" << endl;

		//4、创建接收数据的线程
		// CreateThread和ExitThread是一对，
		// 如果在线程中调用C++运行时库(例如strcpy)，函数会申请空间不释放,ExitThread在退出线程的时候也不会释放空间，就会造成内存泄漏

		// beginthreadex和_endthreadex是一对，
		//_endthreadex会先回收空间，再调用ExitThread，就不会泄露内存
		m_handle = (HANDLE)_beginthreadex(NULL/*安全级别，NULL代表使用默认的安全级别*/,
			0/*线程堆栈大小，0代表使用默认堆栈大小1M*/,
			&recvThread/*线程函数的起始地址，线程函数就是在线程中要执行的函数*/,
			this/*线程函数的参数列表*/,
			0/*线程初始状态，0是创建即运行，CREATE_SUSPENDED是挂起状态，需要调用ResumeThread 来执行此线程*/,
			NULL/*线程id，是输出参数，不需要可以不接*/);

		return true;
	}
}


	//接收数据的线程函数
	unsigned __stdcall Udp::recvThread(void* lpVoid) {
		Udp* pThis = (Udp*)lpVoid;
		pThis->recvData();
		return 1;
	}

			
	//关闭网络(回收线程资源，关闭套接字、卸载库)
void Udp::unInitNet() {
	//创建线程的时候，操作系统会给每个线程分配三个资源：线程ID,线程句柄，内核对象。初始化引用计数器是2
	//操作系统管理资源是引用计数器的方式，引用计数器为0的时候自动回收
	//1.结束线程工作(让循环自动退出)，计数器减一。
	m_bRunning = false;
	
	//2.关闭句柄，计数器减一。
	if (m_handle) {
		if (WAIT_TIMEOUT /*等待超时，等了这么长时间线程还没有退出的话*/ ==
			WaitForSingleObject(m_handle/*等待的哪个线程*/, 500/*等待了多长时间，单位是毫秒*/)) {
			//强制结束线程，但是不要一开始就强制,县城里可能是一些共享的资源，枷锁未释放，结束了就打不开了。
			TerminateThread(m_handle, -1);
		}
		CloseHandle(m_handle);
		m_handle = nullptr;
	}
	//3.关闭套接字
	if (m_sock && INVALID_SOCKET!=m_sock) {
		closesocket(m_sock);
	}
	//4.卸载库
	WSACleanup();
}

	//发送数据
bool Udp::sendData(char* data, int len, unsigned long to) {
	// 判断数据的合法性
	if (!data||len <= 0)
	{
		cout << "Udp::snedData paramater error"<<endl;
		return false;
	}

	//发送数据
	sockaddr_in addrTo;
	addrTo.sin_family = AF_INET;
    addrTo.sin_port = htons(_DEF_UDP_PORT);
	addrTo.sin_addr.S_un.S_addr = to;
	int sendNum = sendto(m_sock,data,len,0,(sockaddr*)&addrTo,sizeof(addrTo));
	if (SOCKET_ERROR == sendNum) {
		cout << "Udp::sendData sendto error" <<WSAGetLastError() << endl;
		return false;
	}
	return true;
}

	//接收数据(阻塞函数,放在线程中运行	)
void Udp::recvData() {
	char buf[8192]="";
	int recvNum = 0;
	sockaddr_in addrFrom;
	int addrSize = sizeof(addrFrom);
	while (m_bRunning) {
		recvNum = recvfrom(m_sock, buf, sizeof(buf), 0, (sockaddr*)&addrFrom, &addrSize);
		if (recvNum>0) {
			//接收数据成功,申请到了一块新的空间
			char* packBuf = new char[recvNum];
			//把接收到的数据拷贝到新空间中
			//strcpy只能拷贝字符串，字符串以\0为结束标志，拷贝过程中遇到0会直接停止
			// 用strcpy拷贝数据很可能导致遇到0而提前停止，导致数据的不完整
			// memcpy只看拷贝数据的大小，不看其中的内容
			memcpy(packBuf,buf,recvNum);

			//把新空间的数据传给中介者类,调用中介者类的函数
			m_pNetMediator->recvData(packBuf, recvNum, addrFrom.sin_addr.S_un.S_addr);
		}
		else {
			cout << "Udp::recvData recvfrom error" <<WSAGetLastError() << endl;
			break;
		}
	}
}	
