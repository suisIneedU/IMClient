#include "ckernel.h"
#include"./mediator/TcpClientMediator.h"
#include<QMessageBox>
#include<QDebug>
#include<QTextCodec>
#include<QInputDialog>

CKernel::CKernel(QObject *parent) : QObject(parent),m_id(0)
{
    setProtocolFun();
    //new登录&注册 的界面对象
    m_pLoginDlg = new LoginDialog;
    //显示界面
    m_pLoginDlg->show();
    //绑定注册信息的信号和槽函数
    connect(m_pLoginDlg,&LoginDialog::sig_registerData,
            this,&CKernel::slot_registerData);
    //绑定登录信息的信号和槽函数
    connect(m_pLoginDlg,&LoginDialog::sig_loginData,
            this,&CKernel::slot_loginData);
    //绑定退出进程的信号和槽函数
    connect(m_pLoginDlg,&LoginDialog::sig_closeProcess,
            this,&CKernel::slot_closeProcess);
    //new好友列表的 界面对象
    m_pFriendList = new FriendList;

    //绑定下线的信号和槽函数
    connect(m_pFriendList,&FriendList::sig_addFriend,
            this,&CKernel::slot_addFriend);

    //绑定添加好友的信号和槽函数
    connect(m_pFriendList,&FriendList::sig_offline,
            this,&CKernel::slot_offline);
    //new中介者类对象
    m_pMediator = new TcpClientMediator;
    //连接、转发所有接收到的数据 给处理类的信号和槽函数
    connect(m_pMediator,&TcpClientMediator::sig_dealData,
            this,&CKernel::slot_dealData);
    //打开Tcp协议客户端的网络
    if(!m_pMediator->openNet()){
        QMessageBox::about(m_pLoginDlg,"提示","打开网络失败");
        exit(1);
    }
}

CKernel::~CKernel()
{
    //1.回收资源
    if(m_pMediator){
        m_pMediator->closeNet();
        delete m_pMediator;
        m_pMediator = nullptr;
    }
    if(m_pLoginDlg){
        m_pLoginDlg->hide();
        delete m_pLoginDlg;
        m_pLoginDlg = nullptr;
    }
    if(m_pFriendList){
        m_pFriendList->hide();
        delete m_pFriendList;
        m_pFriendList = nullptr;
    }

    ChatDialog* chat = nullptr;
    for(auto ite = m_mapIdToChatDlg.begin();ite!=m_mapIdToChatDlg.end();){
        //取出迭代器结点的窗口
        chat = ite.value();
        //回收窗口资源
        chat->hide();
        delete chat;
        chat = nullptr;
        //把无效节点从map从移除
        ite = m_mapIdToChatDlg.erase(ite);
    }
}

QString CKernel::gb2312ToUtf8(char *src)
{
    QTextCodec* gc = QTextCodec::codecForName("gb2312");
    return gc -> toUnicode(src);
}

void CKernel::utf8ToGb2312(QString src, char *dst, int len)
{
    QTextCodec* gc = QTextCodec::codecForName("gb2312");
    QByteArray ba = gc->fromUnicode(src);
    strcpy_s(dst,len,ba.data());
}

//初始化函数指针数组
void CKernel::setProtocolFun(){
    qDebug()<<__func__<<endl;

    //数组初始化
    memset(m_vectorProtocolFun,0,sizeof(m_vectorProtocolFun));

    //把函数地址保存到数组中
    m_vectorProtocolFun[_DEF_REGISTER_RS	-_PROTOCOL_BASE-1] = &CKernel::dealRegisterRs;
    m_vectorProtocolFun[_DEF_LOGIN_RS		-_PROTOCOL_BASE-1] = &CKernel::dealLoginRs;
    m_vectorProtocolFun[_DEF_FRIEND_INFO	-_PROTOCOL_BASE-1] = &CKernel::dealFriendInfo;
    m_vectorProtocolFun[_DEF_OFFLINE_RQ     -_PROTOCOL_BASE-1] = &CKernel::dealOfflineRq;
    m_vectorProtocolFun[_DEF_CHAT_RQ        -_PROTOCOL_BASE-1] = &CKernel::dealChatRq;
    m_vectorProtocolFun[_DEF_CHAT_RS        -_PROTOCOL_BASE-1] = &CKernel::dealChatRs;
    m_vectorProtocolFun[_DEF_ADD_FRIEND_RQ  -_PROTOCOL_BASE-1] = &CKernel::dealAddFriendRq;
    m_vectorProtocolFun[_DEF_ADD_FRIEND_RS  -_PROTOCOL_BASE-1] = &CKernel::dealAddFriendRs;
}

//处理注册回复
void CKernel::dealRegisterRs(char *data, int len, unsigned long from)
{
    qDebug()<<__func__;
    //1、拆包
    _STRU_REGISTER_RS* rs = (_STRU_REGISTER_RS*)data;

    //2、根据注册结果提示用户
    switch(rs->result){
        case _def_register_success:
            QMessageBox::about(m_pLoginDlg,"提示","注册成功");
            break;
        case _def_register_tel_exist:
            QMessageBox::about(m_pLoginDlg,"提示","注册失败,电话号已经被注册");
            break;
        case _def_register_name_exist:
            QMessageBox::about(m_pLoginDlg,"提示","注册失败,昵称已经被占用");
            break;
        default:
            break;
    }
}

//处理登录回复
void CKernel::dealLoginRs(char *data, int len, unsigned long from)
{
    qDebug()<<__func__;
    //1、拆包
    _STRU_LOGIN_RS* rs = (_STRU_LOGIN_RS*)data;
    //2、更具登录结果跳转
    switch(rs->result){
        case _def_login_success:
        {
        m_id = rs->userId;
            //页面跳转，隐藏登陆界面，显示好友列表
            m_pLoginDlg->hide();
            m_pFriendList->show();

        }
            break;
        case _def_login_tel_not_exist:
            QMessageBox::about(m_pLoginDlg,"提示","输入的电话号码未注册");
            break;
        case _def_login_password_error:
            QMessageBox::about(m_pLoginDlg,"提示","密码错误");
            break;
        default:
            break;

    }
}

//处理自己和好友的信息
void CKernel::dealFriendInfo(char *data, int len, unsigned long from)
{
    qDebug()<<__func__;
    //1、拆包
    _STRU_FRIEND_INFO* info = (_STRU_FRIEND_INFO*)data;
    QString name = gb2312ToUtf8(info->name);
    QString feeling = gb2312ToUtf8(info->feeling);
    //2、判断是不是自己的信息，如果是自己的信息
    if(info -> id== m_id){
        //保存自己的昵称
        m_name= name;
        //把自己的信息设置到列表上面
        m_pFriendList->setUserInfo(name,feeling,info->iconId);
        return;
    }
    //3、是好友的信息，是否已经在列表上
    if(m_mapIdToFriendForm.count(info->id)>0){
        //好友的信息已经在列表上 取出好友对象
        friendForm* form = m_mapIdToFriendForm[info->id];
        //更新好友信息
        form->setFriendInfo(info->id,name,feeling,info->iconId,info->status);
    }else{
        //先new一个好友
        friendForm* form = new friendForm;
        //设置好友信息
        form->setFriendInfo(info->id,name,feeling,info->iconId,info->status);
        //把好友添加到列表上
        m_pFriendList->addFriend(form);
        //把好友对象保存到map中
        m_mapIdToFriendForm[info->id] = form;
        //绑定显示窗口的信号和槽
        connect(form,&friendForm::sig_showChatDialog,
                this,&CKernel::slot_showChatDialog);
        //new一个和这个好友的聊天框
        ChatDialog* chat = new ChatDialog;
        //设置窗口的信息
        chat-> setChatDialogInfo(info->id,name);
        //吧聊天窗口保存到Map中
        m_mapIdToChatDlg[info->id] = chat;
        //绑定发送聊天内容的信号和槽函数
        connect(chat,&ChatDialog::sig_sendChat,
                this,&CKernel::slot_sendChat);

    }
}

//处理下线请求
void CKernel::dealOfflineRq(char *data, int len, unsigned long from)
{
    qDebug()<<__func__;
    //1、拆包
    _STRU_OFFLINE_RQ* rq = (_STRU_OFFLINE_RQ*)data;

    //2、找到A客户端的friendForm
    if(m_mapIdToFriendForm.count(rq->userId)>0){
        //取出A客户端的friendForm
        friendForm* form = m_mapIdToFriendForm[rq->userId];
        //设置好友A的状态为下线状态，并且头像变黑
        form->setFriendOffline();
    }
}
//处理聊天请求
void CKernel::dealChatRq(char *data, int len, unsigned long from)
{
    qDebug()<<__func__;
    //1、拆包
    _STRU_CHAT_RQ* rq = (_STRU_CHAT_RQ*)data;

    //2、判断窗口是否存在
    if(m_mapIdToChatDlg.count(rq->userId)>0){
        //3、取出聊天窗口
        ChatDialog* chat = m_mapIdToChatDlg[rq->userId];
        //4、把聊天内容设置到窗口上
        chat->setChat(rq->content);
        //5、显示聊天窗口
        chat->showNormal();
    }
}

//处理聊天回复
void CKernel::dealChatRs(char *data, int len, unsigned long from)
{
    qDebug()<<__func__;
    //1、拆包
    _STRU_CHAT_RS* rs = (_STRU_CHAT_RS*)data;

    //2、判断窗口是否存在

    if(m_mapIdToChatDlg.count(rs->friendId)>0){
        //3、取出聊天窗口
        ChatDialog* chat = m_mapIdToChatDlg[rs->friendId];
        //4、显示用户不在线
        chat->setFriendOffline();
        //5、显示聊天窗口
        chat->showNormal();
    }
}

//处理添加好友请求
void CKernel::dealAddFriendRq(char *data, int len, unsigned long from)
{
    qDebug()<<__func__;
    //1、拆包
    _STRU_ADD_FRIEND_RQ* rq = (_STRU_ADD_FRIEND_RQ*)data;

    //弹出询问窗口 是否同意添加好友
    _STRU_ADD_FRIEND_RS rs;
    if(QMessageBox::Yes == QMessageBox::question(m_pFriendList,"好友请求",QString("【%1】想成为你列表里的一份子").
                                                 arg(rq->userName))){
        rs.result = _def_add_friend_success;
    }else{
        rs.result = _def_add_friend_refuse;
    }
    rs.userId = m_id;
    rs.friendId = rq->userId;
    strcpy_s(rs.friendName,sizeof(rs.friendName),rq->friendName);
    //3、吧添加结果发给服务端
    m_pMediator->sendData((char*)&rs,sizeof(rs),456);
}

//处理添加好友回复
void CKernel::dealAddFriendRs(char *data, int len, unsigned long from)
{
    qDebug()<<__func__;
    //1、拆包
    _STRU_ADD_FRIEND_RS* rs = (_STRU_ADD_FRIEND_RS*)data;
    QString friendName = gb2312ToUtf8(rs->friendName);
    //2、根据结果提示用户
    switch(rs->result){
    case _def_add_friend_success:
        QMessageBox::about(m_pFriendList,"提示",QString("添加【%1】成功").arg(friendName));
        break;
    case _def_add_friend_not_exist:
        QMessageBox::about(m_pFriendList,"提示",QString("添加【%1】失败，不存在这个人").arg(friendName));
        break;
    case _def_add_friend_refuse:
        QMessageBox::about(m_pFriendList,"提示",QString("添加【%1】失败，ta拒绝了你^^").arg(friendName));
        break;
    case _def_add_friend_offline:
        QMessageBox::about(m_pFriendList,"提示",QString("添加【%1】失败，offline").arg(friendName));
        break;
    default:
        break;
    }
}
//处理、分发所有接收到的数据
void CKernel::slot_dealData(char *data, int len, unsigned long from)
{
    qDebug()<<__func__;
    //取出结构体中的type值
    int type = *(int*)data;
    //根据type值计算数组下标
    int index = type - _PROTOCOL_BASE -1;

    //判断数组下标是否越界
        if (index>= 0 && index < _DEF_PROTOCOL_COUNT) {
            //根据数组下标取出函数地址
            PFUN pf = m_vectorProtocolFun[index];
            //判断函数地址是否为空，不为空就调用对应的处理函数
            if (pf) {
                (this->*pf)(data, len, from);
            }
            else {
                qDebug()<< "函数地址为空,type:" <<type<< endl;
            }
        }
        else {
            //type值特别大:1、offset没清零 2、结构体定义的时候没赋值 3、定义的时候没放在最开头
            qDebug() << "type:" << type << endl;
        }
}

//把注册数据传给服务端
void CKernel::slot_registerData(QString name, QString tel, QString passWord)
{
    qDebug()<<__func__;
    //1、打包
    _STRU_REGISTER_RQ rq;
    //strcpy_s(rq.name,sizeof(rq.name),name.toStdString().c_str());
    utf8ToGb2312(name,rq.name,sizeof(rq.name));
    strcpy_s(rq.tel,sizeof(rq.tel),tel.toStdString().c_str());
    strcpy_s(rq.password,sizeof(rq.password),passWord.toStdString().c_str());
    //2、发给服务端
    m_pMediator->sendData((char*)&rq,sizeof(rq),78);
}

//把登录信息发给服务端
void CKernel::slot_loginData(QString tel, QString passWord)
{
    qDebug()<<__func__;
    //1、打包
    _STRU_LOGIN_RQ rq;
    strcpy_s(rq.tel,sizeof(rq.tel),tel.toStdString().c_str());
    strcpy_s(rq.password,sizeof(rq.password),passWord.toStdString().c_str());
    //2、发给服务端
    m_pMediator->sendData((char*)&rq,sizeof(rq),78);
}

//处理退出程序的信号
void CKernel::slot_closeProcess()
{
    qDebug()<<__func__;
    //1.回收资源
    if(m_pMediator){
        m_pMediator->closeNet();
        delete m_pMediator;
        m_pMediator = nullptr;
    }
    if(m_pLoginDlg){
        m_pLoginDlg->hide();
        delete m_pLoginDlg;
        m_pLoginDlg = nullptr;
    }
    if(m_pFriendList){
        m_pFriendList->hide();
        delete m_pFriendList;
        m_pFriendList = nullptr;
    }

    ChatDialog* chat = nullptr;
    for(auto ite = m_mapIdToChatDlg.begin();ite!=m_mapIdToChatDlg.end();){
        //取出迭代器结点的窗口
        chat = ite.value();
        //回收窗口资源
        chat->hide();
        delete chat;
        chat = nullptr;
        //把无效节点从map从移除
        ite = m_mapIdToChatDlg.erase(ite);
    }
    //2.退出进程
    exit(0);
}

//处理下线的信号
void CKernel::slot_offline()
{
    qDebug()<<__func__;
    //1、给我服务端发送下线请求
    _STRU_OFFLINE_RQ rq;
    rq.userId = m_id;
    m_pMediator -> sendData((char*)&rq,sizeof(rq),67);
    //2、回收资源 退出进程
    slot_closeProcess();
}

//处理显示与该好友的聊天窗口的信号
void CKernel::slot_showChatDialog(int friendId)
{
    qDebug()<<__func__;
    //1、判断map中是否有这个窗口
    if(m_mapIdToChatDlg.count(friendId)>0){
        //2、取出聊天窗口
        ChatDialog* chat = m_mapIdToChatDlg[friendId];
        //3、显示
        chat->showNormal();
    }
}

//把聊天内容发给服务端
void CKernel::slot_sendChat(int friendId, QString content)
{
    qDebug()<<__func__;
    //1、打包
    _STRU_CHAT_RQ rq;
    rq.userId =  m_id;
    rq.friendId = friendId;
    strcpy_s(rq.content,sizeof(rq.content),content.toStdString().c_str());
    //2、发送给服务端
    m_pMediator->sendData((char*)&rq,sizeof(rq),67);
}

//处理添加好友的信号
void CKernel::slot_addFriend()
{
    qDebug()<<__func__;
    //1、弹出输入窗口 让用户输入好友的昵称
    QString name = QInputDialog::getText(m_pFriendList,"添加好友","请输入好友的昵称: ");
    QString nameTemp = name;
    //2、检查用户输入的昵称是否合法
    if(name.isEmpty() || nameTemp.remove(" ").isEmpty()){
        QMessageBox::about(m_pFriendList,"提示","不能输入空或者是全空格");
    }
    //3、输入的是否是自己的
    if(name == m_name){
        QMessageBox::about(m_pFriendList,"提示","交朋友要和别人交");
    }
    //4、是否是已经添加的
    for(auto ite = m_mapIdToFriendForm.begin();ite!= m_mapIdToFriendForm.end();ite++){
        //取出好友
        friendForm* form = ite.value();
        if(name == form->friendName()){
            QMessageBox::about(m_pFriendList,"提示","ta已经是你的好友了");
            return;
        }
    }

    //5、给服务端发送请求
    _STRU_ADD_FRIEND_RQ rq;
    rq.userId = m_id;
    //strcpy_s(rq.friendName,sizeof(rq.friendName),name.toStdString().c_str());
    utf8ToGb2312(name,rq.friendName,sizeof(rq.friendName));
    strcpy_s(rq.userName,sizeof(rq.userName),m_name.toStdString().c_str());
    m_pMediator->sendData((char*)&rq,sizeof(rq),456);
}
