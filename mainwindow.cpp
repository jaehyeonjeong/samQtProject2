#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clientmanager.h"
#include "productmanager.h"
#include "shoppingmanager.h"
#include "chetting.h"
#include "tcpclient.h"
#include "tcpserver.h"


#include <QMdiSubWindow>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    clientmanager = new ClientManager(this);
    productmanager = new ProductManager(this);
    shoppingmanager = new ShoppingManager(this);

    //    TCPServer* tcpserver;
    //    TCPClient* tcpclient;
    tcpserver = new TCPServer(this);
    tcpclient = new TCPClient(this);

    chettingapp = new Chetting(this);


    ui->tabWidget->addTab(clientmanager, tr("&ClientTab"));
    ui->tabWidget->addTab(productmanager, tr("&ProductTab"));
    //ui->tabWidget->addTab(shoppingmanager, tr("&ShoppingTab"));

    //    QMdiArea mdiArea;
    //    QMdiSubWindow *subWindow1 = new QMdiSubWindow;
    //    subWindow1->setWidget(internalWidget1);
    //    subWindow1->setAttribute(Qt::WA_DeleteOnClose);
    //    mdiArea.addSubWindow(subWindow1);

    //    QMdiSubWindow *subWindow2 =
    //        mdiArea.addSubWindow(internalWidget2);

    subWindow = new QMdiSubWindow;
    subWindow->setWidget(shoppingmanager);
    subWindow->setAttribute(Qt::WA_DeleteOnClose);
    subWindow->setWindowTitle("Shopping Window");
    subWindow->setGeometry(0, 0, 550, 400);
    ui->mdiArea->addSubWindow(subWindow);

    /*TCP 채팅 위젯 추가*/
    TcpSubWindow[0] = new QMdiSubWindow;
    TcpSubWindow[0]->setWidget(tcpserver);
    TcpSubWindow[0]->setAttribute(Qt::WA_DeleteOnClose);
    TcpSubWindow[0]->setWindowTitle("TcpServer");
    ui->mdiArea->addSubWindow(TcpSubWindow[0]);
    TcpSubWindow[0]->setGeometry(550,0, 240, 100);

    TcpSubWindow[1] = new QMdiSubWindow;
    TcpSubWindow[1]->setWidget(tcpclient);
    TcpSubWindow[1]->setAttribute(Qt::WA_DeleteOnClose);
    TcpSubWindow[1]->setWindowTitle("client chetting");
    ui->mdiArea->addSubWindow(TcpSubWindow[1]);
    TcpSubWindow[1]->setGeometry(0, 400, 300, 350);

    TcpSubWindow[2] = new QMdiSubWindow;
    TcpSubWindow[2]->setWidget(chettingapp);
    TcpSubWindow[2]->setAttribute(Qt::WA_DeleteOnClose);
    TcpSubWindow[2]->setWindowTitle("manager Chetting Application");
    ui->mdiArea->addSubWindow(TcpSubWindow[2]);
    TcpSubWindow[2]->setGeometry(300, 400, 490, 350);


    /*탭과 MDIAREA 스핀로드*/
    QList<int> list;
    list << 700 << 800;
    ui->splitter->setSizes(list);

    /*메인윈도우에서 데이터 커넥트*/
    connect(clientmanager, SIGNAL(ClientAdded(QString)),
            shoppingmanager, SLOT(CreceiveData(QString)));

    connect(productmanager, SIGNAL(ProductAdded(QString)),
            shoppingmanager, SLOT(PreceiveData(QString)));

    connect(productmanager, SIGNAL(ProductPrices(QString)),
            shoppingmanager, SLOT(PreceivePrice(QString)));

    this->resize(1500, 800);

    //    connect(chettingapp->clientsocket,
    //            &QAbstractSocket::errorOccurred,
    //            [=]{qDebug() << chettingapp->clientsocket->errorString();
    //    });
    //    connect(chettingapp->clientsocket, SIGNAL(readyRead()), SLOT(rechoData()));

    connect(clientmanager, SIGNAL(ClientAdded(QString)),
            tcpclient, SLOT(CReceiveData(QString)));

    connect(tcpclient, SIGNAL(ButtonSignal(QString)),
            chettingapp, SLOT(receiveClient(QString)));

    connect(tcpclient, SIGNAL(ButtonSignal(QString)),
            chettingapp, SLOT(receiveClientName(QString)));/*에러*/
}

MainWindow::~MainWindow()
{
    delete ui;
    delete clientmanager;
    delete productmanager;
    delete shoppingmanager;

    delete tcpclient;
    delete tcpserver;

    for(int i = 0; i < 3; i++)
    {
        delete TcpSubWindow[i];
    }
}

