#include"TcpServer.h"
#include"../mediator/TcpServerMediator.h"
TcpServer::TcpServer(INetMediator* p):m_sock(INVALID_SOCKET), m_bRunning(true) {
    m_pNetMediator = p;
}
TcpServer::~TcpServer() {

}

//初始化网络(加载库、创建套接字、绑定ip端口、监听、创建接受连接的线程)
bool TcpServer::initNet() {
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

    //3、绑定ip和端口号
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    // 魔鬼数字:把数字定义成宏
    addr.sin_port = htons(_DEF_TCP_PORT);    // 转换成网络字节序(大端)
    // 绑定所有网卡
    addr.sin_addr.S_un.S_addr = ADDR_ANY;
    err = bind(m_sock, (sockaddr*)&addr, sizeof(addr));
    if (SOCKET_ERROR == err) {
        cout << "bind error:" << WSAGetLastError() << endl;
        return false;
    }
    else {
        cout << "bind success" << endl;
    }
    //4.监听
    err = listen(m_sock, _DEF_TCP_LISTEN_MAX);
    if (SOCKET_ERROR == err) {
        cout << "listen error: " << WSAGetLastError() << endl;
        return false;
    }else {
        cout << "listen success" << endl;
    }
    // 5.接受连接的线程的函数
    HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, &acceptThread, this, 0, NULL);
    if(handle) {
        m_listHandle.push_back(handle);
    }
    return true;
}

//接受链接的线程函数(循环接受链接)
unsigned __stdcall TcpServer::acceptThread(void* lpVoid) {
    TcpServer* pThis = (TcpServer*)lpVoid;
    SOCKET sock = INVALID_SOCKET;
    sockaddr_in addrFrom ;
    int addrSize = sizeof(addrFrom);
    HANDLE handle = nullptr;
    unsigned int threadId = 0;
    while (pThis->m_bRunning) {
        //接受连接	
        sock = accept(pThis->m_sock, (sockaddr*)&addrFrom, &addrSize);
        if (INVALID_SOCKET == sock) {
            //接受连接失败
            cout<<"accept error:" <<WSAGetLastError()<< endl;
            }else{
            //连接成功，先打印客户端的 IP
            cout<<"accept success:" <<inet_ntoa(addrFrom.sin_addr)<< endl;

            //给当前连接成功的客户端创建一个接收数据的线程
            handle = (HANDLE)_beginthreadex(nullptr, 0, &recvThread, pThis, 0, &threadId);
            //保存线程句柄
            if (handle) {
                pThis->m_listHandle.push_back(handle);
            }
            //保存线程id和客户端socket
            pThis->m_mapThreadIdToSocket[threadId] = sock ;
        }
    }
    return 1;
}


//接受数据的线程函数
unsigned __stdcall TcpServer::recvThread(void* lpVoid) {
    TcpServer* pThis = (TcpServer*)lpVoid;
    pThis->recvData();
    return 1;
}

//关闭网络(回收线程资源、关闭套接字n+1个、卸载库)
void TcpServer::unInitNet(){
    //1.结束线程工作(让循环自动退出)，计数器减一。
    m_bRunning = false;

    //2.关闭句柄，计数器减一。
    HANDLE handle = nullptr;
    for (auto ite = m_listHandle.begin(); ite != m_listHandle.end();) {
        //取出节点中的句柄
        handle = *ite;
        //回收句柄对应的资源
        if (handle) {
            if (WAIT_TIMEOUT == WaitForSingleObject(handle, 500)) {
                TerminateThread(handle, -1);
            }
            CloseHandle(handle);
            handle = nullptr;
        }
        //移除当前迭代器指向的无效节点(返回值是下一个有效节点)
        ite=m_listHandle.erase(ite);
    }
    if (handle) {
        if (WAIT_TIMEOUT == WaitForSingleObject(handle, 500)) {
            TerminateThread(handle, -1);
        }
        CloseHandle(handle);
        handle = nullptr;
    }

    //3.关闭套接字
    if (m_sock && INVALID_SOCKET != m_sock) {
        closesocket(m_sock);
    }
    SOCKET sock = INVALID_SOCKET;
    for (auto ite = m_mapThreadIdToSocket.begin(); ite != m_mapThreadIdToSocket.end();) {
        //取出socket
        sock = ite->second;
        //回收socket
        if (sock&&INVALID_SOCKET != sock) {
            closesocket(sock);
        }
        //移除当前迭代器指向的无效节点(返回值是下一个有效节点)
        ite=m_mapThreadIdToSocket.erase(ite);
    }
    //4.卸载库
    WSACleanup();
}

//发送数据(想发给谁发给谁)
bool TcpServer::sendData(char* data, int len, unsigned long to) {
    //1.校验参数和发行
    if (!data ||len<=0) {
        cout << "TcpServer::sendData paramater error" << endl;
        return false;
    }
    //2.先发包长度
    int sendNum = send(to,(char*)&len, sizeof(len), 0);
    if (sendNum == SOCKET_ERROR) {
        cout<<"TcpServer::sendData send1 error" <<WSAGetLastError() << endl;
        return false;
    }
    //3.再发包内容
    sendNum = send(to, data, len, 0);
    if (SOCKET_ERROR == sendNum) {
        cout<< "TcpServer::sendData send2 error" << WSAGetLastError()<<endl;
        return false;
    }
    return true;
}

//接收数据(阻塞函数，放在线程里面运行)
void TcpServer::recvData() {

    //休眠一会儿(给acceptThread一点时间把socket保存到map中，不然下面从map中取值的时候可能会失败)
    Sleep(5);
    //取出当前线程对应的socket
    unsigned int threadId =	GetCurrentThreadId();
    //从map中取值的时候首先要判断map中是否有key
    SOCKET sock = INVALID_SOCKET;
    if (m_mapThreadIdToSocket.count(threadId)>0) {
        sock = m_mapThreadIdToSocket[threadId];
    }
    //判断获取到的socket是否有效
    if (!sock||INVALID_SOCKET==sock) {
        cout<<"TcpServer::recvData socket error" << endl;
        return;
    }


    //保存recv的返回值
    int recvNum = 0;
    //保存包的大小
    int packSize = 0;
    //记录一个包中累计接收到了多少数据
    int offset = 0;
    while (m_bRunning) {
        //接收包大小
        recvNum = recv(sock, (char*)&packSize, sizeof(packSize), 0);
        if (recvNum > 0) {
            //接收包长度成功，接收包内容
            //按照包的大小申请接收包内容的空间
            char* packBuf = new char[packSize];
            //开始接收包内容
            while (packSize/*记录包中还剩余多少容量*/ > 0) {
                recvNum/*表示一次接收到的数据量*/ = recv(sock, packBuf + offset, packSize, 0);
                if (recvNum > 0) {
                    //接收一次数据成功，累计数据量增加，剩余空间减少
                    offset += recvNum;
                    packSize -= recvNum;
                }
                else {
                    cout << "TcpServer::recvData recv2 error" << endl;
                    break;
                }
            }
            //跳出循环
            //把接收到的数据发给中介者类别
            m_pNetMediator->recvData(packBuf, offset, sock);
    
            //offset清零
            offset = 0;
        }
        else {
            cout << "TcpServer::recvData reval error: " << WSAGetLastError() << endl;
            break;
        }
    }
}
