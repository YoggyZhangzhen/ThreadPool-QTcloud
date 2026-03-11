#include "clienttask.h"


#include <QTcpSocket>
#include <QThread>

ClientTask::ClientTask()
{

}

ClientTask::ClientTask(MyTcpSocket *socket)
{
    m_pSocket=socket;
}

void ClientTask::run()
{
    connect(m_pSocket, &QTcpSocket::readyRead, m_pSocket, &MyTcpSocket::recvMsg);
    connect(m_pSocket, &QTcpSocket::disconnected, m_pSocket, &MyTcpSocket::clientOffline);
    m_pSocket->moveToThread(QThread::currentThread());
}
