#ifndef CKERNEL_H
#define CKERNEL_H

#include<QMap>
#include <QObject>
#include"chatdialog.h"
#include"net/def.h"
#include"logindialog.h"
#include"friendlist.h"
#include"./mediator/INetMediator.h"
#include"friendform.h"
class CKernel;
//定义函数指针类型
typedef void (CKernel::*PFUN)(char* data, int len, unsigned long from);

class CKernel : public QObject
{
    Q_OBJECT
public:
    explicit CKernel(QObject *parent = nullptr);
    ~CKernel();

    //gb2312转utf8
    QString gb2312ToUtf8(char* src);
    //utf8转gb2312
    // src utf8编码方式的原始数据
    // dst 转换gb2312编码方式的数据要存储的空间
    // 空间的大小
    void utf8ToGb2312(QString src, char* dst,int len);

    //初始化函数指针数组
    void setProtocolFun();

    //处理注册回复
    void dealRegisterRs(char* data, int len, unsigned long from);
    //处理登录回复
    void dealLoginRs(char* data, int len, unsigned long from);
    //处理自己和好友的信息
    void dealFriendInfo(char* data, int len, unsigned long from);
    //处理下线请求
    void dealOfflineRq(char* data, int len, unsigned long from);
    //处理聊天请求
    void dealChatRq(char* data, int len, unsigned long from);
    //处理聊天回复
    void dealChatRs(char* data, int len, unsigned long from);
    //处理添加好友请求
    void dealAddFriendRq(char* data, int len, unsigned long from);
    //处理添加好友回复
    void dealAddFriendRs(char* data, int len, unsigned long from);

public slots:
    //处理、分发所有接收到的数据
     void slot_dealData(char* data, int len, unsigned long from);

     //把注册数据传给服务端
     void slot_registerData(QString name,QString tel,QString password);

     //把登录信息发给服务端
     void slot_loginData(QString tel,QString password);

     //处理退出程序的信号
     void slot_closeProcess();

     //处理下线的信号
     void slot_offline();

     //处理显示与该好友的聊天窗口的信号
     void slot_showChatDialog(int firendId);

     //把聊天内容发给服务端
     void slot_sendChat(int friendId,QString content);

     //处理添加好友的信号
     void slot_addFriend();
signals:

private:
     //保存当前登录用户的id
     int m_id;
     //保存当前登录用户的昵称
     QString m_name;
    INetMediator* m_pMediator;
    //登录&注册界面
    LoginDialog* m_pLoginDlg;
    //好友列表界面
    FriendList* m_pFriendList;
    //函数指针数组
    PFUN m_vectorProtocolFun[_DEF_PROTOCOL_COUNT];
    //保存好友id和好友对象fiendForm
    QMap<int,friendForm*>m_mapIdToFriendForm;
    //保存好友id和聊天窗口对象
    QMap<int,ChatDialog*>m_mapIdToChatDlg;
};

#endif // CKERNEL_H
