#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class QSystemTrayIcon;
class QLabel;
class QPixmap;
class QTimer;
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    QSystemTrayIcon * icon;
    QLabel *canvas;
    QPixmap *natsu;
    QTimer * timer;
    QTimer * record;
    int idx;
    int cnt;
    enum class Type{
        loading,
        idle,
        goodbye,
        exit,
        drag,
        drag_over,
        clicked,
        clicked_over,
    };
    Type t;
    bool isDragging;
    bool isPressed;
    QPoint dragPosition;

    void setIcon();
    void setMovieInit();
    //void setType(QString);
    float setRate();
    void setMovie(QString,std::function<void()> = [](){});
    void updateFrame();
    void selectIdle();
    void mouseMoveEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;

signals:
    void movieChange();
};
#endif // WIDGET_H
