#include "clienttask.h"
#include "mytcpserver.h"

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr handle)
{
    qDebug() << "新的客户端连接";
    //每一个客户端连接，都将其socket加入列表
    MyTcpSocket* pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(handle);
    m_tcpSocketList.append(pTcpSocket);
    connect(pTcpSocket, &MyTcpSocket::offline, this, &MyTcpServer::deleteSocket);

    ClientTask *task= new ClientTask(pTcpSocket);
    m_threadPool.start(task);
}

void MyTcpServer::resend(char *caTarName, PDU *pdu)
{
    if(caTarName == NULL || pdu == NULL){
        return;
    }

    for(int i=0 ; i<m_tcpSocketList.size();i++)
        if(caTarName == m_tcpSocketList.at(i)->m_strLoginName){
            m_tcpSocketList.at(i)->write((char*)pdu,pdu->uiPDULen);
            qDebug() << "readMsg uiMsgType:" << pdu->uiMsgType
                     <<"uiPDULen:"<<pdu->uiPDULen
                     <<"uiPDULen:"<<pdu->uiPDULen
                     <<"caData:"<<pdu->caData
                     <<"caMsg:"<<pdu->caMsg;
            break;
        }
}

void MyTcpServer::deleteSocket(MyTcpSocket *mysocket)
{
    m_tcpSocketList.removeOne(mysocket);
    mysocket->deleteLater();
    mysocket = NULL;

    qDebug() << m_tcpSocketList.size();
    foreach(MyTcpSocket* pSocket, m_tcpSocketList){
        qDebug() << pSocket->m_strLoginName;
    }
//    if(mysocket == NULL)
}

MyTcpServer::MyTcpServer()
{
    m_threadPool.setMaxThreadCount(16);
}
