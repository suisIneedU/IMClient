#ifndef FRIENDFORM_H
#define FRIENDFORM_H

#include <QWidget>

namespace Ui {
class friendForm;
}

class friendForm : public QWidget
{
    Q_OBJECT
signals:
    //发送信号 通知kernel显示窗口
    void sig_showChatDialog(int friendId);
public:
    explicit friendForm(QWidget *parent = nullptr);
    ~friendForm();

    //保存并设置好友的信息
    void setFriendInfo(int id,QString name,QString feeling,
                       int iconId,int status);
    //设置好友的状态为下线状态，并且头像变黑
    void setFriendOffline();
    const QString &friendName() const;

    void setFriendName(const QString &newFriendName);

private slots:
    void on_pb_icon_clicked();

private:
    Ui::friendForm *ui;
    int m_friendId;
    QString m_friendName;
    QString m_feeling;
    int m_iconId;
    int m_status;
};

#endif // FRIENDFORM_H
