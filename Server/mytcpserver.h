#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include "mytcpsocket.h"

#include <QObject>
#include <QTcpServer>
#include <QThreadPool>

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    static MyTcpServer& getInstance();
    void incomingConnection(qintptr handle);
    void resend(char* caTarName, PDU *pdu);
    QThreadPool m_threadPool;


public slots:
    void deleteSocket(MyTcpSocket* mysocket);


private:
    MyTcpServer();
    MyTcpServer(const MyTcpServer& instance) = delete;
    MyTcpServer& operator=(const MyTcpServer&) = delete;
    QList<MyTcpSocket*> m_tcpSocketList;

};

#endif // MYTCPSERVER_H
