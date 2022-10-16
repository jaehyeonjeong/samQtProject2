#include "chetting.h"
#include "ui_chetting.h"
#include "tcpclient.h"

#include <QtNetwork>
#include <QMessageBox>
#include <QDataStream>
#include <QTreeWidgetItem>

#define BLOCK_SIZE 1024

Chetting::Chetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chetting)
{
    ui->setupUi(this);

    /*tcp_server*/
    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::Any, 8001);
    connect(tcpServer, SIGNAL(newConnection()), SLOT(clientConnect()));
//    if(!tcpServer->listen())
//    {
//        QMessageBox::critical(this, tr("Echo Server"),
//                              tr("Unable to start the server: %1.")\
//                              .arg(tcpServer->errorString()));
//        close();
//        return;
//    }

    ui->serverstatus->setText(tr("The server is running on port %1.")
                              .arg(tcpServer->serverPort()));

    ui->textmessage->setReadOnly(true);

    /*tcp client*/
    ui->ipAddress->setText("127.0.0.1");
    QRegularExpression re("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$|");

    QRegularExpressionValidator validator(re);
    ui->ipAddress->setValidator(&validator);
    ui->ipAddress->setPlaceholderText("Server IP Address");

    ui->portNumber->setText("8000");
    ui->portNumber->setInputMask("00000;_");
    ui->portNumber->setPlaceholderText("Server Port No");

    /*소켓도 항시 초기화가 필요함*/
    clientSocket = new QTcpSocket(this);
    connect(ui->connectButton,
            &QPushButton::clicked,
            [=]{clientSocket->connectToHost(
                    ui->ipAddress->text(),
                    ui->portNumber->text().toInt());
                    connectToServer();      //서버 커넥트
    });

    /*connect 버튼을 누를시 server이름이 받아짐*/
    connect(ui->connectButton, SIGNAL(clicked()),
            this, SLOT(receiveClientName(QString)));/*에러*/


    //서버로 보낼 메시지를 위한 위젯들
    connect(ui->inputEdit, SIGNAL(returnPressed()), SLOT(sendData()));
    connect(ui->inputEdit, SIGNAL(returnPressed()),
            ui->inputEdit, SLOT(clear()));

    connect(ui->sendButton, SIGNAL(clicked()), SLOT(sendData()));
    connect(ui->sendButton, SIGNAL(clicked()), ui->inputEdit, SLOT(clear()));


    connect(ui->cacelButton,
            &QPushButton::clicked,
            [=]{ui->inputEdit->setText("");});

    connect(clientSocket,
            &QAbstractSocket::errorOccurred,
            [=]{qDebug() << clientSocket->errorString();});
    connect(clientSocket, SIGNAL(readyRead()), SLOT(receiveData()));
    connect(clientSocket, SIGNAL(disconnected()), SLOT(disconnect()));

    ui->IPNameEdit->setText("No names");
}

/*tcp server*/
void Chetting::echoData()
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);
    foreach(QTcpSocket *sock, serverList) {
        if(sock != clientConnection)
            sock->write(bytearray);
    }
    ui->serverstatus->setText(QString(bytearray));

}

/*id 인트형으로 자동 할당*/
int Chetting::makeID()
{
    if(tcpdataList.size() == 0)
    {
        return 1;
    }
    else
    {
        auto id = tcpdataList.lastKey();
        return ++id;
    }
}

void Chetting::receiveTcpClientName(QString name)/*입력과 같은예시*/
{
    /*버튼을 누를시 받을 슬롯*/
    int id = makeID();
    name = ui->IPNameEdit->text();
    if(name.length())
    {
        tcpdata* tcp = new tcpdata(id, name);
        tcpdataList.insert(id, tcp);
        ui->treeWidget->addTopLevelItem(tcp);
    }

}

void Chetting::receiveClient(QString name)
{
    ui->IPNameEdit->setText(name);
}

void Chetting::clientConnect()
{
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection( );
    connect(clientConnection, SIGNAL(disconnected( )), \
            clientConnection, SLOT(deleteLater( )));
    connect(clientConnection, SIGNAL(readyRead( )), SLOT(echoData( )));
    //connect(clientConnection, SIGNAL(disconnected()), SLOT(removeItem()));
    ui->serverstatus->setText("new connection is established...");

    serverList.append(clientConnection);        // QList<QTcpSocket*> clientList;
}

void Chetting::removeItem()
{
    QTcpSocket* clientConnection = dynamic_cast<QTcpSocket*>(sender());
    serverList.removeOne(clientConnection);
    clientConnection->deleteLater();
}

/*tcp client and protocol*/
void Chetting::closeEvent(QCloseEvent*)
{
    chattingProtocol data;
    data.type = 3;
    qstrcpy(data.data, ui->IPNameEdit->text().toStdString().data());

    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << data.type;
    out.writeRawData(data.data, 1020);
    clientSocket->write(sendArray);

    clientSocket->disconnectFromHost();
    while(clientSocket->waitForDisconnected(1000))
        QThread::usleep(10);
}

void Chetting::receiveData( )
{
    QTcpSocket *clientSocket = dynamic_cast<QTcpSocket *>(sender( ));
    if (clientSocket->bytesAvailable( ) > BLOCK_SIZE) return;
    QByteArray bytearray = clientSocket->read(BLOCK_SIZE);
    chattingProtocol data;
    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in >> data.type;
    in.readRawData(data.data, 1020);

    qDebug( ) << data.type;
    switch(data.type) {
    case 2:
        ui->textmessage->append(QString(data.data));
        break;
    case 4:
        QMessageBox::critical(this, tr("Chatting Client"), \
                              tr("Invited from Server"));
        break;
    };
}

void Chetting::sendData(  )
{
    QString str = ui->inputEdit->text( );
    if(str.length( )) {
        QByteArray bytearray;
        bytearray = str.toUtf8( );
        ui->textmessage->append("<font color=red>나</font> : " + str);

        chattingProtocol data;
        data.type = 2;
        qstrcpy(data.data, bytearray.data());

        QByteArray sendArray;
        QDataStream out(&sendArray, QIODevice::WriteOnly);
        out << data.type;
        out.writeRawData(data.data, 1020);
        clientSocket->write(sendArray);
    }
}

void Chetting::connectToServer( )
{
    chattingProtocol data;
    data.type = 1;
    qstrcpy(data.data, ui->IPNameEdit->text().toStdString().data());

    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << data.type;
    out.writeRawData(data.data, 1020);
    clientSocket->write(sendArray);
}

void Chetting::disconnect( )
{
    QMessageBox::critical(this, tr("Chatting Client"), \
                          tr("Disconnect from Server"));
//    close( );
}

//void Chetting::CReceiveData(QString str)
//{
//    ui->IPNameEdit->setText(str);
//}

Chetting::~Chetting()
{
    delete ui;
    delete tcpServer;
    delete clientSocket;
}

/*reduce 버튼을 누를 시 List데이터가 없어지면서 해당 servername도 나가지는 방법 고안*/
/*list->treeWidget으로 변경 계획*/
void Chetting::on_reduceclient_clicked()
{
//    QModelIndex index;//= ui->IPNameEdit->text().toStdString().data();
//    QString ipname;
//    ipname = ui->IPNameEdit->text().toStdString().data();
//    QListWidgetItem* item = ui->listWidget->currentItem();
//    if(item != nullptr)
//    {
//        ui->listWidget->indexFromItem
//                (ui->listWidget->itemFromIndex(index));
//        ui->listWidget->update();
//    }
/*리스트로는 제거가 되지 않아서 TreeWidget으로 데이터를 조작*/
    /*reduce button을 누르면 해당 이름이 삭제*/
    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    if(item != nullptr)
    {
        tcpdataList.remove(item->text(0).toInt());
        ui->treeWidget->takeTopLevelItem
                (ui->treeWidget->indexOfTopLevelItem(item));
        ui->treeWidget->update();
    }

    /*강제 추방기능은 여기서 기능을 구현하면 될 것 같음*/
}


//void Chetting::on_listWidget_itemClicked(QListWidgetItem *item)
//{
//    ui->reduceEdit->setText(item->text());
//}


void Chetting::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    ui->reduceEdit->setText(item->text(0));
    /*제거 에디트 위젯에서 해당 되는것은 이름뿐이라 일단 name만 넣음*/
}

