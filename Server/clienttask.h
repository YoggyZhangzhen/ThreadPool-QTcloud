#ifndef CLIENTTASK_H
#define CLIENTTASK_H

#include <QObject>
#include <QRunnable>
#include "mytcpsocket.h"

class ClientTask : public QObject,public QRunnable
{
    Q_OBJECT
public:
    ClientTask();
    ClientTask(MyTcpSocket*socket);
    MyTcpSocket*m_pSocket;
    void run() override;
};

#endif // CLIENTTASK_H
