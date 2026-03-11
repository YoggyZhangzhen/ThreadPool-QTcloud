#include "mytcpserver.h"
#include "server.h"
#include "ui_server.h"

#include <QFile>
#include <QDebug>

Server::Server(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);
    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP), m_usPort);
}

Server &Server::getInstance()
{
    static Server instance;
    return instance;
}

Server::~Server()
{
    delete ui;
}

QString Server::getRootDir()
{
    return m_strRootDir;
}

void Server::loadConfig()
{
    QFile file(":/server.config");
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        QString strData = QString(baData);
        QStringList strList =  strData.split("\r\n");
        m_strIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();
        m_strRootDir=strList.at(2);
        file.close();
        qDebug() << "loadConfig m_strIP:" << m_strIP
                 << "m_usPort:" << m_usPort
                 << "m_strRootDir:" << m_strRootDir;
    }else{
        qDebug() << "打开配置失败";
    }

}
