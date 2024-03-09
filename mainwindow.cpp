#include "mainwindow.h"
#include <QString>
#include <QDebug>
#include <QCoreApplication>

#define DIEPATH QCoreApplication::applicationDirPath().append("/res/hit.wav")
#define WINGPATH QCoreApplication::applicationDirPath().append("/res/wing.wav")
#define BGMPATH QCoreApplication::applicationDirPath().append("/res/bgm.wav")
#define POINTPATH QCoreApplication::applicationDirPath().append("/res/point.wav")

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent)
{
    init();
    initGame();
}

MainWindow::~MainWindow()
{
    delete  pipeChannels;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    //qDebug() << "paintEvent";
    drawBack();
    drawPipe();
    drawBird();
}

void MainWindow::mousePressEvent(QMouseEvent * )
{
    // qDebug() << "click";

    //鸟上升时扑打翅膀的声音
    //QSound::play(WINGPATH);

    birdStatus = BirdStatus::UP;
    initSpeed();
}

//初始化
void MainWindow::init()
{
    QSound::play(BGMPATH);

    //this->move(400, 300);

    this->setFixedSize(480, 640);
    qDebug() << "init ";

    // 使用绝对路径
    ground = QPixmap(
        QCoreApplication::applicationDirPath().append("/res/ground.png")
        //
        );
    if(!ground){
        qDebug() << "ground is null" << endl;
    }

    // 缩放
    ground = ground.scaled(20,80);
    int groundHeight = ground.height();
    background = QPixmap(
        QCoreApplication::applicationDirPath().append("/res/background.png")
        //
        );

    //缩放
    background = background.scaled(
        this->width(), this->height() - groundHeight
        //
        );

    //防止除不尽有余数，所以加1
    groundSize = this->width()/ground.width() + 1;

    v = new QVector<int>(groundSize);

    for(int i = 0; i < groundSize; i++){
        int x = i * ground.width();
        (*v)[i] = x;
    }

    QPixmap bird(
        QCoreApplication::applicationDirPath().append("/res/bird.png")
        //
        );

    bird1 = bird.copy(0, 0, 92, 64);
    bird1 = bird1.scaled(60, 42);

    bird2 = bird.copy(92, 0, 92, 64);
    bird2 = bird2.scaled(60, 42);

    bird3 = bird.copy(92*2, 0, 92, 64);
    bird3 = bird3.scaled(60, 42);

    scoreLabel = new QLabel(this);
    scoreLabel->setGeometry(
        width()/2,
        0, 50,
        50
        //
        );

    //第一个属性是字体（微软雅黑），第二个是大小，第三个是加粗（权重是75）
    QFont font ( "Microsoft YaHei", 20, 75);
    scoreLabel->setFont(font);
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::white);
    scoreLabel->setPalette(pa);
    this->score = 0;
    scoreLabel->setText(QString::number(this->score));

    //最高分数设置
    scoreLabel1 = new QLabel(this);
    scoreLabel1->setGeometry(
        0, this->height() - 50, 250,
        50
        //
        );

    scoreLabel1->setFont(font);
    scoreLabel1->setPalette(pa);
    scoreLabel1->setText("best score " + QString::number(best.score));

    startButton = new QPushButton(this);

    //雅黑/第一个属性是字体（微软），第二个是大小，第三个是加粗（权重是75）
    font = QFont( "Microsoft YaHei", 25, 75);

    startButton->setFont(font);
    startButton->setPalette(pa);
    startButton->setText(QString("start"));

    startButton->setGeometry(
        width()/2 -50, height()/2 -30,
        100, 60
        //
        );

    // startButton->hide();

    timer = new QTimer(this);

    //事件
    connect(
        timer,SIGNAL(timeout()),
        this,SLOT(loopPaint())
        //
        );

    connect(
        startButton,SIGNAL(clicked()),
        this,SLOT(slotStartGame())
        //
        );
}

//初始化游戏相关
void MainWindow::initGame()
{
    best.init();
    birdX = this->width() / 3;
    birdY = this->height() / 2 - bird1.height();

    this->score = 0;
    scoreLabel->setText(QString::number(this->score));
    scoreLabel1->setText("best score " + QString::number(best.score));

    uint seed_x = static_cast<uint>(clock());

    //获取随机数
    int l = GlobalUtils::getRandomNum(seed_x,200);
    if(pipeChannels == nullptr){

        //创建两个通道
        pipeChannels = new QVector<PipeChannel*>(2);

    }else {

        delete (*pipeChannels)[0];
        delete (*pipeChannels)[1];
    }
    //窗口高度 底下管道高度 x位置横坐标 地面高度 objet parent
    (*pipeChannels)[0] = new PipeChannel(
        height(), 100+l, width()+100, ground.height(),
        this
        //
        );

    (*pipeChannels)[1] = new PipeChannel(
        height(), 150 +l, 2 * width() + l, ground.height(),
        this
        //
        );
}

//初始化速度
void MainWindow::initSpeed()
{
    birdDownSpeed = 2.0;
    birdUpSpeed = 5.0;
}

//开始游戏
void MainWindow::startGame()
{
    QSound::play(POINTPATH);
    startButton->hide();
    initGame();
    initSpeed();
    gameStatus = GameStatus::RUNNING;
    birdStatus = BirdStatus::DOWN;
    timer->start(16);
}

//停止游戏
void MainWindow::stopGame()
{
    timer->stop();
    gameStatus = GameStatus::STOPING;
    startButton->show();

    if(score > best.score)
    {
        best.score = score;
        best.save();
    }
}

//绘制背景
void MainWindow::drawBack()
{
    QPainter painter(this);
    int height = ground.height();
    int pos;

    // 绘制背景图
    painter.drawPixmap(0,0, background);

    // 绘制地面
    for(int i = 0; i < groundSize; i++)
    {
        pos = (*v)[i];

        //绘制一个地面图像
        painter.drawPixmap(pos,this->height() - height, ground);

        // 改变坐标 移动起来
        pos -= moveSpeed;
        if(pos <= -ground.width()){
            pos = (groundSize-1) *  ground.width();
        }

        (*v)[i] = pos;
    }
}

//绘制bird
void MainWindow::drawBird()
{

    QPainter painter(this);
    QPixmap bird = bird1;
    if(gameStatus == GameStatus::RUNNING){

        //绘制哪一个 (动画效果)
        if(birdNo < birdNoMax/3){
            bird = bird1;
        }else if(birdNo < birdNoMax/3 * 2){
            bird = bird2;
        }else{
            bird = bird3;
        }
        birdNo++;
        if(birdNo > birdNoMax){
            birdNo = 1;
        }
    }

    // 小鸟下降
    if(birdStatus == BirdStatus::DOWN){

        birdY += birdDownSpeed;
        birdDownSpeed+=0.1;

        //下降的同时旋转
        //matrix.rotate(1);
        //bird = bird.transformed(matrix, Qt::SmoothTransformation);
    }

    // 判断是否碰撞
    if(isCrush()){

        QSound::play(DIEPATH);
        stopGame();

    }

    // 上升
    if(birdStatus == BirdStatus::UP){

        birdY -= birdUpSpeed;
        birdUpSpeed -= 0.2;

        //matrix.rotate(-1);
        //bird = bird.transformed(matrix, Qt::SmoothTransformation);
        if(birdUpSpeed <= 0.0){
            birdStatus = BirdStatus::DOWN;
            // qDebug() << "down";
            initSpeed();
        }
    }
    painter.drawPixmap(birdX, birdY, bird);
}

//绘制管道
void MainWindow::drawPipe()
{
    QPainter painter(this);
    uint seed_x = static_cast<uint>(clock());

    //获取随机数
    int l = GlobalUtils::getRandomNum(seed_x,200);

    //另一个通道的位置， 只有两个管道
    int otherX = 0;
    for(int i = 0; i < pipeChannels->size(); i++)
    {
        PipeChannel * pipeChannel = (*pipeChannels)[i];
        if(i ==0)
        {
            otherX = (*pipeChannels)[1]->getX();
        }else {
            otherX = (*pipeChannels)[0]->getX();
        }

        pipeChannel->draw(painter);
        int x = pipeChannel->getX();

        if(x + pipeChannel->getPipeWidth() < birdX && !pipeChannel->isScore)
        {
            QSound::play(POINTPATH);

            //增加得分
            this->score++;
            scoreLabel->setText(QString::number(this->score));
            pipeChannel->isScore = true;
        }

        // 改变坐标移动起来
        x -= moveSpeed;

        // 移出窗口了
        if(x <= -pipeChannel->getPipeWidth())
        {
            //重新设置位置
            x = otherX + width() / 2 +  2 * l;

            //重新设置高度
            pipeChannel->setH1(50+l);

            //重新初始化
            pipeChannel->initPipe();
        }

        pipeChannel->setX(x);
    }
}

//碰撞检测
bool MainWindow::isCrush()
{

    //是否碰撞地面
    if (birdY >= this->height() - ground.height() - bird1.height()){
        return true;
    }

    //是否碰撞天空
    //if(birdY <= 0)return true;

    //是否碰撞管道
    for(PipeChannel* c : *pipeChannels){
        if(
            birdX + bird1.width() > c->getX() &&
            birdX < c->getX() + c->getPipeWidth() &&
            (birdY < c->getPiPe2Height() || birdY+bird1.height() > c->getY1())
            //
            ){
            return true;
        }
    }

    return false;
}

//循环绘制
void MainWindow::loopPaint(){
    update();
}

//循环游戏
void MainWindow::slotStartGame(){
    startGame();
}
