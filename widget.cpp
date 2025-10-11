#include "widget.h"
#include "ui_widget.h"
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QTimer>
#include <QPixmap>
#include <QRandomGenerator>

#define SCALE 500
#define PREFIX ":/asset/"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    // 设置窗口为无边框 透明 置顶
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    this->raise();

    setIcon();
    setMovieInit();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setType(QString key){
    if(key=="loading"){
        t = Type::loading;
    }
    else if(key=="idle0"|| key=="idle1"){
        t = Type::idle;
    }
    else if(key=="goodbye"){
        t = Type::goodbye;
    }
    else if(key=="exit"){
        t= Type::exit;
    }

}

float Widget::setRate(){
    switch(t){
    case Type::loading:
        return 1;
    case Type::idle:
        return 1.875;
    default:
        return 1;
    }
}

void Widget::setMovieInit(){
    canvas = new QLabel(this);
    canvas->setScaledContents(true);
    canvas->resize(SCALE,SCALE);
    natsu = new QPixmap();
    natsu->load(PREFIX+QString("loading.png"));
    setType("loading");
    timer = new QTimer(this);
    idx =0;
    cnt = natsu->width()/SCALE;
    connect(timer,&QTimer::timeout,this,[this](){
        int x = idx*SCALE;
        QPixmap frame = natsu->copy(x, 0, SCALE, SCALE);
        canvas->setPixmap(frame);
        idx ++;
        if(idx==cnt){
            emit movieChange();
            return;
        }
        idx %= cnt ;
    });
    timer->start(100*setRate());
    connect(this,&Widget::movieChange,this,[this](){
        // 随机数 in [0,1)
        double ran = QRandomGenerator::global()->generateDouble();
        switch(t){
        case Type::goodbye:
            setMovie("exit");
            break;
        case Type::loading:
        case Type::idle:
            if(ran<1){ //TODO
                //qDebug()<<"q\n";
                setMovie("idle0");
            }
            else{
                //qDebug()<<"a\n";
                setMovie("idle1");
            }
            break;
        case Type::exit:
            QApplication::quit();
        default:
            break;
        }


    });
}

void Widget::setMovie(QString str){
    setType(str);
    QString path = PREFIX + str + QString(".png");
    natsu->load(path);
    timer->setInterval(100*setRate());
    idx = 0;
    cnt = natsu->width()/SCALE;
}

void Widget::setIcon(){
    // setWindowFlags(Qt::Tool);
    icon = new QSystemTrayIcon(this);
    icon->setIcon(QIcon(":/asset/NatsuSystemTray.ico"));
    auto menu = new QMenu(this);
    auto quitAct = new QAction(this);
    quitAct->setText("退出");
    connect(quitAct,&QAction::triggered,this,[this](){
        setMovie("goodbye");
        //QApplication::quit();
    });
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


