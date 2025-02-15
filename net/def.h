#pragma once
#include<string.h>

// 定义UDP协议的端口号
#define  _DEF_UDP_PORT (1234)

// 定义TCP协议的端口号
#define _DEF_TCP_PORT (4321)

// 定义TCP协议的监听最大连接数
#define _DEF_TCP_LISTEN_MAX			 (100)

//昵称和密码的最大长度
#define _DEF_NAME_MAX_LENGTH		 (20)

//电话号码的最大长度
#define _DEF_TEL_MAX_LENGTH			 (12)

//签名的最长长度
#define _DEF_FEELING_MAX_LENGTH		 (100)

//聊天内容最长长度
#define _DEF_CHAT_CONTENT_MAX_LENGTH (4096)
//函数指针数组大小（协议头个数）
#define _DEF_PROTOCOL_COUNT			 (10)
//定义协议头
#define _PROTOCOL_BASE				 (1000)
//注册请求
#define _DEF_REGISTER_RQ			 (_PROTOCOL_BASE + 1)
//注册回复
#define _DEF_REGISTER_RS			 (_PROTOCOL_BASE + 2)
//登陆请求
#define _DEF_LOGIN_RQ				 (_PROTOCOL_BASE + 3)
//登陆回复
#define _DEF_LOGIN_RS				 (_PROTOCOL_BASE + 4)
//添加好友请求
#define _DEF_ADD_FRIEND_RQ			 (_PROTOCOL_BASE + 5)
//添加好友回复
#define _DEF_ADD_FRIEND_RS			 (_PROTOCOL_BASE + 6)
//显示好友列表
#define _DEF_FRIEND_INFO			 (_PROTOCOL_BASE + 7)
//聊天请求
#define _DEF_CHAT_RQ				 (_PROTOCOL_BASE + 8)
//聊天回复
#define _DEF_CHAT_RS				 (_PROTOCOL_BASE + 9)
//下线请求
#define _DEF_OFFLINE_RQ				 (_PROTOCOL_BASE + 10)


//定义请求结果
//注册结果
#define _def_register_success		(0)
#define _def_register_tel_exist		(1)
#define _def_register_name_exist	(2)

//登陆结果
#define _def_login_success			(0)
#define _def_login_tel_not_exist	(1)
#define _def_login_password_error	(2)

//添加好友结果
#define _def_add_friend_success		(0)
#define _def_add_friend_not_exist	(1)
#define _def_add_friend_refuse		(2)
#define _def_add_friend_offline		(3)

//用户状态
#define _def_status_online			(0)
#define _def_status_offline			(1)

//聊天结果
#define _def_chat_success			(0)
#define _def_chat_fail				(1)

//定义请求结构体
// 注册请求:手机号 昵称 密码
typedef struct _STRU_REGISTER_RQ {
    _STRU_REGISTER_RQ():type(_DEF_REGISTER_RQ){
        memset(tel,0, _DEF_TEL_MAX_LENGTH);
        memset(name, 0, _DEF_NAME_MAX_LENGTH);
        memset(password, 0, _DEF_NAME_MAX_LENGTH);
    }
    int type;
    char tel[_DEF_TEL_MAX_LENGTH];
    char name[_DEF_NAME_MAX_LENGTH];
    char password[_DEF_NAME_MAX_LENGTH];
}_STRU_REGISTER_RQ;

//注册回复:注册结果 (失败:手机号已被注册，昵称已被注册)或者成功
typedef struct _STRU_REGISTER_RS {
    _STRU_REGISTER_RS():type(_DEF_REGISTER_RS),result(_def_register_name_exist){}
    int type;
    int result;
}_STRU_REGISTER_RS;

// 登录请求:电话号码 、密码
typedef struct _STRU_LOGIN_RQ {
    _STRU_LOGIN_RQ():type(_DEF_LOGIN_RQ) {
        memset(tel, 0, _DEF_TEL_MAX_LENGTH);
        memset(password, 0, _DEF_NAME_MAX_LENGTH);
    }
    int type;
    char tel[_DEF_TEL_MAX_LENGTH];
    char password[_DEF_NAME_MAX_LENGTH];
}_STRU_LOGIN_RQ;

//登录回复:登陆的结果(成功，手机号未注册,密码错误)
typedef struct _STRU_LOGIN_RS {
    _STRU_LOGIN_RS() :result(_def_login_password_error),type(_DEF_LOGIN_RS) ,userId(0){}
    int type;
    int result;
    int userId;
}_STRU_LOGIN_RS;

// 添加好友请求:只支持昵称添加好友   好友的昵称、自己的昵称、自己的ID
typedef struct _STRU_ADD_FRIEND_RQ {
    _STRU_ADD_FRIEND_RQ():type(_DEF_ADD_FRIEND_RQ),userId(0) {
        memset(friendName,0, _DEF_NAME_MAX_LENGTH);
        memset(userName,0, _DEF_NAME_MAX_LENGTH);
    }
    int type;
    char friendName[_DEF_NAME_MAX_LENGTH];
    char userName[_DEF_NAME_MAX_LENGTH];
    int userId;
}_STRU_ADD_RQ;

//添加好友回复：添加结果(成功，昵称不存在、拒绝了、不在线),好友的昵称,好友的id，自己的id
typedef struct _STRU_ADD_FRIEND_RS {
    _STRU_ADD_FRIEND_RS() :result(_def_add_friend_refuse),
        friendId(0),userId(0),type(_DEF_ADD_FRIEND_RS){
        memset(friendName,0, _DEF_NAME_MAX_LENGTH);
    }
    int type;
    int result;
    int userId;
    int friendId;
    char friendName[_DEF_NAME_MAX_LENGTH];
}_STRU_ADD_FRIEND_RS;

// 显示好友列表:好友的id，昵称，签名，头像id，状态
typedef struct _STRU_FRIEND_INFO {
    _STRU_FRIEND_INFO():id(0), iconId(0), status(_def_status_offline),type(_DEF_FRIEND_INFO) {
        memset(feeling, 0, _DEF_FEELING_MAX_LENGTH);
        memset(name, 0, _DEF_NAME_MAX_LENGTH);
    }
    int type;
    int id;
    int iconId;
    int status;
    char name[_DEF_NAME_MAX_LENGTH];
    char feeling[_DEF_FEELING_MAX_LENGTH];
}_STRU_FRIEND_INFO;

// 聊天请求:聊天内容,自己的id，好友的id
typedef struct _STRU_CHAT_RQ {
    _STRU_CHAT_RQ():type(_DEF_CHAT_RQ),userId(0), friendId(0) {
        memset(content, 0, _DEF_CHAT_CONTENT_MAX_LENGTH);
    }
    int type;
    char content[_DEF_CHAT_CONTENT_MAX_LENGTH];
    int userId;
    int friendId;
}_STRU_CHAT_RQ;

//聊天回复:发送结果(发送成功,发送失败)
typedef struct _STRU_CHAT_RS {
    _STRU_CHAT_RS() :type(_DEF_CHAT_RS),result(_def_chat_fail),friendId(0){}
    int type;
    int result;
    int friendId;
}_STRU_CHAT_RS;

// 下线请求:自己的id
typedef struct _STRU_OFFLINE_RQ {
    _STRU_OFFLINE_RQ() :type(_DEF_OFFLINE_RQ),userId(0){}
    int type;
    int userId;
}_STRU_OFFLINE_RQ;

