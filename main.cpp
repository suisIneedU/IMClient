#include "ckernel.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CKernel kernel;
    return a.exec();
}



//QT的信号和槽机制(自带的 自定义的--通知事件&传输数据)
//1、满足基础条件：两个类必须都继承自QObject 并且有Q_OBJECT宏

//2、定义信号：发送数据的类 或者通知事情发生的类在头文件中使用signals声明信号，
//返回值必须是void，参数根据需要填写，但是信号不需要在cpp中实现，在需要的地方使用Q_EMIT发送数据

//3、实现槽函数:在接收数据或者接受事件的类头文件中使用 pubic slots 声明槽函数，参数和返回值要和槽函数一致，在cpp中实现

//4、连接信号和槽函数：在槽函数的类中，在发送信号的对象new出来以后连接信号和槽函数

//字符串:char[](基础类型) 、std::string(C++类) 、QString(QT类)
//基础类型可以直接给类赋值:
//char tel[]; std::string a = tel;     QString b =tel;

//std::string(C++类) 调用c_str()函数可以转换成char[]

//QString调用tStdString()函数转换成std::string
