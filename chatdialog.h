#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT
signals:
    //把聊天内容发给kernel
    void sig_sendChat( int friendId,QString content);
public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();

    //保存并设置当前聊天窗口的信息
    void setChatDialogInfo(int id,QString name);

    //设置聊天内容到窗口上
    void setChat(QString content);

    //设置好友不在线
    void setFriendOffline();

private slots:
    void on_pb_send_clicked();

private:
    Ui::ChatDialog *ui;
    int m_friendId;
    QString m_friendName;
};

#endif // CHATDIALOG_H
