#include "chatdialog.h"
#include "ui_chatdialog.h"
#include<QTime>
ChatDialog::ChatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}
//发送按钮
void ChatDialog::on_pb_send_clicked()
{
    //1、获取用户输入的纯文本
    QString content = ui->te_chat->toPlainText();
    //2、判断是否为空或者全空格
    if(content.isEmpty() || content.remove(" ").isEmpty()){
        ui->te_chat->setText("");
        return;
    }
    //3、重新获取用户输入的带格式的内容
    content = ui->te_chat->toHtml();
    //4、清空编辑窗口
    ui->te_chat->setText("");
    //5、吧内容显示到浏览窗口上
    ui->tb_chat->append(QString("【我】%1").arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
    //6、把聊天内容发送给kernel
    Q_EMIT sig_sendChat(m_friendId,content);
}

//保存并设置当前聊天窗口的信息
void ChatDialog::setChatDialogInfo(int id, QString name)
{
    m_friendId = id;
    m_friendName  = name;
    setWindowTitle(QString("同%1的会话").arg(m_friendName));
}

//设置聊天内容到窗口上
void ChatDialog::setChat(QString content)
{
    ui->tb_chat->append(QString("【%1】 %2 ").arg(m_friendName).arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
}

//设置好友不在线
void ChatDialog::setFriendOffline()
{
    ui->tb_chat->append(QString("【%1】 %2 Offline").arg(m_friendName).arg(QTime::currentTime().toString("hh:mm:ss")));
}
