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
#include <QThread>

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

    // 设置拖拽
    isDragging = false;
    isPressed = false;
    record = new QTimer(this);
    record->setSingleShot(true);
    connect(record,&QTimer::timeout,this,[this](){
        isDragging = true;
        t = Type::drag;
        emit movieChange();
    });
}

Widget::~Widget()
{
    delete ui;
}

// void Widget::setType(QString key){
//     if(key=="loading"){
//         t = Type::loading;
//     }
//     else if(key=="idle0"|| key=="idle1"){
//         t = Type::idle;
//     }
//     else if(key.slice(0,4)=="drag"){
//         t = Type::drag;
//     }
//     else if(key=="goodbye"){
//         t = Type::goodbye;
//     }
//     else if(key=="exit"){
//         t= Type::exit;
//     }

// }

float Widget::setRate(){
    switch(t){
    case Type::loading:
        return 1;
    case Type::idle:
        return 2.5;
    case Type::drag:
        return 2.25;
    case Type::clicked_over:
        return 2;
    default:
        return 1;
    }
}

void Widget::updateFrame(){
    int x = idx*SCALE;
    QPixmap frame = natsu->copy(x, 0, SCALE, SCALE);
    canvas->setPixmap(frame.scaled(canvas->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    if(idx + 1==cnt){
        timer->stop();
        //qDebug()<<x<<Qt::endl;
        emit movieChange();
    }
    else{
        idx = (idx+1)% cnt ;
    }
}

void Widget::selectIdle(){
    // 随机数 in [0,1)
    double ran = QRandomGenerator::global()->generateDouble();
    if(ran<0.5){ //TODO
        setMovie("idle0");
    }
    else{
        setMovie("idle1");
    }
}

void Widget::setMovieInit(){
    canvas = new QLabel(this);
    // canvas->setScaledContents(true);
    canvas->resize(SCALE/2,SCALE/2);
    setFixedSize(canvas->size());
    natsu = new QPixmap();
    natsu->load(PREFIX+QString("loading.png"));
    t = Type::loading;
    timer = new QTimer(this);
    idx =0;
    cnt = natsu->width()/SCALE;
    connect(timer,&QTimer::timeout,this,&Widget::updateFrame);
    timer->start(100*setRate());
    connect(this,&Widget::movieChange,this,[this](){
        // 随机数 in [0,1)
        double ran = QRandomGenerator::global()->generateDouble();
        switch(t){
        case Type::goodbye:
            t = Type::exit;
            setMovie("exit");
            break;
        case Type::idle:
            selectIdle();
            break;
        case Type::drag:
            if(ran<0.5){
                setMovie("drag0");
            }
            else{
                setMovie("drag1");
            }
            break;
        case Type::exit:
            timer->stop();
            record->stop();
            QApplication::quit();
            return;
        case Type::clicked:
            QTimer::singleShot(1000, this, [this](){
                t = Type::clicked_over;
                setMovie("clicked_over", [this](){
                    // 让 clicked_over 的第一帧停得更久（例如 1000ms），后续恢复正常
                    timer->setInterval(1000);
                    // 下一次 timeout 后恢复为正常速率，并且断开一次性连接
                    auto conn = new QMetaObject::Connection();
                    *conn = connect(timer, &QTimer::timeout, this, [this, conn](){
                        timer->setInterval(100 * setRate()); // clicked_over 正常速率
                        QObject::disconnect(*conn);
                        delete conn;
                    });
                });
            });
            break;
        default:
            t = Type::idle;
            selectIdle();
            break;
        }


    });
}

void Widget::setMovie(QString str, std::function<void()> handle){
    QString path = PREFIX + str + QString(".png");
    natsu->load(path);
    idx = 0;
    cnt = natsu->width()/SCALE;
    updateFrame();
    timer->setInterval(100*setRate());
    //timer->stop();
    timer->start();
    handle();
}

void Widget::setIcon(){
    icon = new QSystemTrayIcon(this);
    icon->setIcon(QIcon(":/asset/NatsuSystemTray.ico"));
    auto menu = new QMenu(this);
    auto quitAct = new QAction(this);
    quitAct->setText("退出");
    connect(quitAct,&QAction::triggered,this,[this](){
        t = Type::goodbye;     
        setMovie("goodbye");  
    });
    menu->addAction(quitAct);
    icon->setContextMenu(menu);
    icon->setToolTip("小夏");
    icon->show();
    connect(icon,&QSystemTrayIcon::activated,this,[this](QSystemTrayIcon::ActivationReason r){
        if(r==QSystemTrayIcon::Trigger){
            this->show();
            this->raise();
            // this->activateWindow();
        }
    });
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    // 只响应左键和特定状态
    if (event->button() == Qt::LeftButton && t != Type::goodbye && t != Type::exit && t != Type::loading) {
        isPressed = true;
        // 记录鼠标相对窗口的位置
        dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        //按下不动，视为拖拽，否则为点击
        record->start(500);
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if (isPressed && (event->buttons() & Qt::LeftButton)) {
        // 计算偏移并移动窗口,很显然，如果有移动也算拖拽
        if(event->globalPosition().toPoint()!=dragPosition && !isDragging){
            isDragging = true;
            t = Type::drag;
            record->stop();
            emit movieChange();
        }
        move(event->globalPosition().toPoint() - dragPosition);
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    record->stop();
    if (event->button() == Qt::LeftButton) {
        if(isDragging){
            isDragging = false;
            t = Type::drag_over;
            setMovie("drag_over");
        }
        // 否则为点击了一次
        else{
            t = Type::clicked;
            setMovie("clicked0");
        }
    }

}


