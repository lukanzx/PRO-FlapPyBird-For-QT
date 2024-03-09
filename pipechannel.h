#ifndef PIPECHANNEL_H
#define PIPECHANNEL_H

#include <QObject>
#include <QPixmap>
#include <QImage>
#include <QPainter>
/**
 * @brief The PipeChannel class
 * 通道
 */
class PipeChannel : public QObject
{
    Q_OBJECT
public:

    //是否已经计分
    bool isScore = false;

    //winHeight 窗口高度
    //h1 底下管道高度
    //x 位置横坐标
    //groundHeight
    //地面高度 parent
    PipeChannel(
        int winHeight,
        int h1,
        int x,
        int groundHeight,
        QObject *parent = nullptr
        //
        );

    //初始化管道
    void initPipe();

    //绘制管道
    void draw(QPainter &painter);

    //setX 设置位置 X坐标
    void setX(int x);

    //getX 获取位置
    int getX();

    //getY1 获取管道1（底部管道）的y坐标
    int getY1();

    //getPiPe2Height 获取管道2的高度
    int getPiPe2Height();

    //获取管道宽度
    int getPipeWidth();

    //设置底下管道高度
    void setH1(int h1);

    ~PipeChannel();

signals:

public slots:
private:

    // 管道图片
    QImage pipeImage;

    //管道图像 镜像
    QImage imageMirror;

    //通道宽度
    int channelWidth = 250;

    //底下管道的高度
    int h1 = 150;

    //管道宽度
    int pipeWidth = 90;

    //底下管道
    QPixmap pipe1;

    //上面管道
    QPixmap pipe2;

    //窗口高度
    //位置横坐标
    //地面高度
    int winHeight,x,groundHeight;
};

#endif // PIPECHANNEL_H
