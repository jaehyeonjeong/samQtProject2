#ifndef TCPDATA_H
#define TCPDATA_H

#include <QTreeWidgetItem>

class tcpdata : public QTreeWidgetItem
{
public:
    tcpdata(int id = 0, QString = "");

    int id() const;
    QString gtcpClientName()const;
    void stcpClientName(QString&);

};

#endif // TCPDATA_H
