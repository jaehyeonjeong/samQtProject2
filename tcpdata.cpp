#include "tcpdata.h"

tcpdata::tcpdata(int id, QString tcname)
{
    setText(0, QString::number(id));
    setText(1, tcname);
}

QString tcpdata::gtcpClientName() const
{
    return text(1);
}

void tcpdata::stcpClientName(QString& name)
{
    setText(1, name);
}
