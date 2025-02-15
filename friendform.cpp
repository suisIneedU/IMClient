#include "friendform.h"
#include "ui_friendform.h"
#include"./net/def.h"
#include<QBitmap>
friendForm::friendForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::friendForm)
{
    ui->setupUi(this);
}

friendForm::~friendForm()
{
    delete ui;
}

//保存并设置好友的信息
void friendForm::setFriendInfo(int id,QString name,QString feeling,
                               int iconId,int status){
    //保存好友的信息到成员变量
    m_friendId = id;
    m_friendName = name;
    m_feeling=  feeling;
    m_iconId =  iconId;
    m_status = status;

    //设置好友的昵称和签名
    ui->lb_name->setText(m_friendName);
    ui->lb_feeling->setText(m_feeling);
    //加载头像文件
    QString path = QString(":/tx/%1.png").arg(m_iconId);
    //判断好友的状态
    if(_def_status_online == m_status){
        //在线 头像亮
        QIcon icon = QIcon(path);
        ui->pb_icon->setIcon(icon);
    }
    else{
        //不在线 头像暗
        QBitmap bp;
        bp.load(path);              //黑白遗照设置方法
        ui->pb_icon->setIcon(bp);
    }
}

//设置好友的状态为下线状态，并且头像变黑
void friendForm::setFriendOffline()
{
    m_status = _def_status_offline;
    QString path = QString(":/tx/%1.png").arg(m_iconId);
    QBitmap bp;
    bp.load(path);              //黑白遗照设置方法
    ui->pb_icon->setIcon(bp);
    this->repaint();
}
//显示与好友的聊天窗口
void friendForm::on_pb_icon_clicked()
{
    //发送信号 通知kernel显示窗口
    Q_EMIT sig_showChatDialog(m_friendId);
}

const QString &friendForm::friendName() const
{
    return m_friendName;
}
