#ifndef CHETTING_H
#define CHETTING_H

#include <QWidget>
#include <QDataStream>

#include "tcpdata.h"

class QTextEdit;
class QLineEdit;
class QTcpSocket;
class QTcpServer;
class QTreeWidgetItem;
class QListWidgetItem;

typedef struct{
    int type;
    char data[1020];
}chattingProtocol;

namespace Ui {
class Chetting;
}

class Chetting : public QWidget
{
    Q_OBJECT

public:
    explicit Chetting(QWidget *parent = nullptr);

    ~Chetting();

private slots:
    /*server slots*/
    void clientConnect();
    void echoData();
    void removeItem();

    /*client slots and protocol*/
    void connectToServer( );
    void receiveData( );			// 서버에서 데이터가 올 때
    void sendData( );			// 서버로 데이터를 보낼 때
    void disconnect( );

    void receiveTcpClientName(QString name);

    void on_reduceclient_clicked();

    void receiveClient(QString);

    //void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

private:
    void closeEvent(QCloseEvent*) override;
    Ui::Chetting *ui;
    /*tcp server*/
    QTcpServer* tcpServer;
    QList<QTcpSocket*> serverList;

    /*tcp client*/
    QTcpSocket* clientSocket;
    //QList<QTcpSocket*> clientList;

    int makeID();
    QMap<int, tcpdata*>tcpdataList; //tcpdata.h에 있는 data리스트 설정
};

#endif // CHETTING_H
