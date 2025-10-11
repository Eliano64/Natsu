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
    int idx;
    int cnt;

    void setIcon();
    void setMovieInit();
    void setType(QString);
    enum class Type{
        loading,
        idle,
        goodbye,
        exit,
    };
    Type t;
    float setRate();
    void setMovie(QString);

signals:
    void movieChange();
};
#endif // WIDGET_H
