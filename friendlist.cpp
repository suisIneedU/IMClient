#include "friendlist.h"
#include "ui_friendlist.h"
#include<QMessageBox>
#include<QDebug>
FriendList::FriendList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FriendList)
{
    ui->setupUi(this);

    // new一个垂直布局的层的对象
    m_pLayout = new QVBoxLayout;
    //设置spaceing和margin
    m_pLayout->setContentsMargins(0,0,0,0);
    m_pLayout->setSpacing(3);
    //把层镶嵌到外面的大控件上
    ui->widget_list->setLayout(m_pLayout);

    //new一个菜单项的对象
    m_pMenu = new QMenu;
    //添加菜单项目
    m_pMenu->addAction("添加好友");
    m_pMenu->addAction("系统设置");
    //绑定点击菜单项的信号和槽函数
    connect(m_pMenu,&QMenu::triggered,
            this,&FriendList::slot_triggered);
}

FriendList::~FriendList()
{
    if(m_pLayout){
       delete m_pLayout;
       m_pLayout = nullptr;
    }
    delete ui;
}

//增加一个好友
void FriendList::addFriend(friendForm* form){
    m_pLayout->addWidget(form);
}
//设置自己的信息
void FriendList::setUserInfo(QString name, QString feeling, int iconId)
{
    ui->lb_name->setText(name);
    ui->le_feeling->setText(feeling);
    QString path = QString(":/tx/%1.png").arg(iconId);
    ui->pb_icon->setIcon(QIcon(path));
}

//重写父类的关闭事件
void FriendList::closeEvent(QCloseEvent *event)
{
      if(QMessageBox::Yes == QMessageBox::question(this,"询问","是否退出程序？")){
          Q_EMIT sig_offline();
      }else{
          //忽略退出程序事件 不忽略仍然会关闭
          event->ignore();
      }
}

//在鼠标点击的位置向上弹出一个菜单项
void FriendList::on_pb_menu_clicked()
{
    //获取鼠标点击的坐标
    QPoint poin = QCursor::pos();
    //获取菜单的绝对大小
    QSize size = m_pMenu->sizeHint();
    //在鼠标点击的位置向上弹出一个菜单项
    m_pMenu->exec(QPoint(poin.x(),poin.y() - size.height()));
}

//点击菜单项的槽函数
void FriendList::slot_triggered(QAction *action)
{
    if("添加好友" == action->text()){
        //通知kernel添加好友
        Q_EMIT sig_addFriend();
    }else if("系统设置" == action->text()){
        qDebug()<<"系统设置";
    }
}

