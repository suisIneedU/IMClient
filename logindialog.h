#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginDialog; }
QT_END_NAMESPACE

class LoginDialog : public QDialog
{
    Q_OBJECT
signals:
    //把注册数据传给kernel
    void sig_registerData(QString name,QString tel,QString password);
    //把登录数据传给kernel
    void sig_loginData(QString tel,QString password);
    //通知kernel退出程序
    void sig_closeProcess();
public:
    LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    //重写父类的关闭事件
    void closeEvent(QCloseEvent *event);

private slots:
    void on_pb_clear_clicked();

    void on_pb_commit_clicked();

    void on_pb_commite_register_clicked();

    void on_pb_clear_register_clicked();

private:
    Ui::LoginDialog *ui;
};
#endif // LOGINDIALOG_H
