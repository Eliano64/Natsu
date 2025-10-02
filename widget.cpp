#include "widget.h"
#include "ui_widget.h"
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QMenu>
#include <QAction>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->raise();

    setIcon();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setIcon(){
    setWindowFlags(Qt::Tool);
    icon = new QSystemTrayIcon(this);
    icon->setIcon(QIcon(":/asset/NatsuSystemTray.ico"));
    auto menu = new QMenu(this);
    auto quitAct = new QAction(this);
    quitAct->setText("退出");
    connect(quitAct,&QAction::triggered,QApplication::instance(),&QApplication::quit);
    menu->addAction(quitAct);
    icon->setContextMenu(menu);
    icon->setToolTip("小夏");
    icon->show();
    connect(icon,&QSystemTrayIcon::activated,this,[this](QSystemTrayIcon::ActivationReason r){
        if(r==QSystemTrayIcon::Trigger){
            // qDebug()<<"Trigger!"<<Qt::endl;
            this->show();
            this->raise();
            // this->activateWindow();
        }
    });
}
