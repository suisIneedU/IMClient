#include "logindialog.h"
#include "ui_logindialog.h"
#include<QMessageBox>


LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

//重写父类的关闭事件
void LoginDialog::closeEvent(QCloseEvent *event)
{
    Q_EMIT sig_closeProcess();
}


void LoginDialog::on_pb_clear_clicked()
{
    ui->le_password->setText("");
    ui->le_tel->setText("");
}


void LoginDialog::on_pb_commit_clicked()
{
    //1、从控件上获取用户输入的数据

    QString tel = ui->le_tel->text();
    QString password = ui->le_password->text();

    QString telTmp       = tel;
    QString passwordTmp  = password;
    //2、校验数据的合法性
    //1、判断字符串是否为空或者是全空格
    if(tel.isEmpty() || password.isEmpty() ||
       telTmp.remove(" ").isEmpty() || passwordTmp.remove(" ").isEmpty()){                                     //remove()会改变数据的原始内容
        QMessageBox::about(this,"提示","不能输入空的内容或者全空格！");
        return;
    }
    //2、判断字符串长度是否合法
    if(password.length()>20 || 11!=tel.length()){
        QMessageBox::about(this,"提示","昵称和密码不能超过20位，电话号码必须是11位数字！");
        return;
    }
    //3、把登录信息发给kernel
    Q_EMIT sig_loginData(tel,password);
}


void LoginDialog::on_pb_commite_register_clicked()
{
    //从控件上获取用户输入的数据
    QString name = ui->le_name_register->text();  //text()返回值是QString
    QString tel = ui->le_tel_register->text();
    QString password = ui->le_password_register->text();
    QString nameTmp      = name;
    QString telTmp       = tel;
    QString passwordTmp  = password;

    //校验数据合法性
    //1、判断字符串是否为空或者是全空格
    if(name.isEmpty() || tel.isEmpty() || password.isEmpty() ||
       nameTmp.remove(" ").isEmpty() || telTmp.remove(" ").isEmpty() || passwordTmp.remove(" ").isEmpty()){                                     //remove()会改变数据的原始内容
        QMessageBox::about(this,"提示","不能输入空的内容或者全空格！");
        return;
    }
    //2、判断字符串长度是否合法
    if(name.length()>20 || password.length()>20 || 11!=tel.length()){
        QMessageBox::about(this,"提示","昵称和密码不能超过20位，电话号码必须是11位数字！");
        return;
    }
    //3、判断字符串内容是否合法
    //for循环嵌套if语句太慢 要用正则表达式--四阶段学习

    //把注册数据传给kernel
    Q_EMIT sig_registerData(name,tel,password);
}


void LoginDialog::on_pb_clear_register_clicked()
{
    ui->le_name_register->setText("");
    ui->le_password_register->setText("");
    ui->le_tel_register->setText("");
}

