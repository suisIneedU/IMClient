#ifndef FRIENDLIST_H
#define FRIENDLIST_H

#include<QMenu>
#include <QDialog>
#include<QCloseEvent>
#include<QVBoxLayout>
#include"friendform.h"
namespace Ui {
class FriendList;
}

class FriendList : public QDialog
{
    Q_OBJECT
signals:
    //通知kernel下线
    void sig_offline();

    //通知kernel添加好友
    void sig_addFriend();
public:
    explicit FriendList(QWidget *parent = nullptr);
    ~FriendList();

    //增加一个好友
    void addFriend(friendForm* form);

    //设置自己的信息
    void setUserInfo(QString name,QString feeling,int iconId);

    //重写父类的关闭事件
    void closeEvent(QCloseEvent *event);
private slots:
    void on_pb_menu_clicked();

    //点击菜单项的槽函数
    void slot_triggered(QAction *action);
private:
    Ui::FriendList *ui;
    //定义垂直布局的层
    QVBoxLayout* m_pLayout;
    //定义一个菜单项
    QMenu* m_pMenu;
};

#endif // FRIENDLIST_H
