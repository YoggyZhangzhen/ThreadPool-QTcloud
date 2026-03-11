#include "client.h"
#include "ui_client.h"
#include "protocol.h"
#include "index.h"
#include <QFile>
#include <QDebug>
#include <QHostAddress>
#include <QMessageBox>
#include <QMessageBox>

Client::Client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Client)
{
    ui->setupUi(this);
    loadConfig();
    connect(&m_tcpSocket, &QTcpSocket::connected, this, &Client::showConnect);
    connect(&m_tcpSocket, &QTcpSocket::readyRead, this, &Client::recvMsg);
    m_tcpSocket.connectToHost(QHostAddress(m_strIP), m_usPort);
    m_prh = new ResHandler;
}

Client &Client::getInstance()
{
    static Client instance;
    return instance;
}

Client::~Client()
{
    delete ui;
}

void Client::loadConfig()
{
    QFile file(":/client.config");
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        QString strData = QString(baData);
        QStringList strList =  strData.split("\r\n");
        m_strIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();
        m_strRootDir = strList.at(2);
        file.close();
        qDebug() << "loadConfig m_strIP:" << m_strIP
                 << "m_usPort:" << m_usPort
                 << "m_strRootDir:"<<m_strRootDir;
    }else{
        qDebug() << "打开配置失败";
    }

}

void Client::sendMsg(PDU *pdu)
{
    m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
    qDebug() << "send uiMsgType:" << pdu->uiMsgType
             <<"uiPDULen:"<<pdu->uiPDULen
             <<"uiPDULen:"<<pdu->uiPDULen
             <<"caData:"<<pdu->caData
             <<"caData+32:"<<pdu->caData+32
             <<"caMsg:"<<pdu->caMsg;
    free(pdu);
    pdu = NULL;
}

PDU *Client::readMsg()
{
    qDebug() << "\n\nreadMsg 接收消息长度： " << m_tcpSocket.bytesAvailable();
    uint uiPDULen = 0;
    m_tcpSocket.read((char*)&uiPDULen, sizeof (uint));//读协议总长度

    //要分两步，先要知道总长度，才能确定后面要读多少
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU* pdu = mkPDU(0, uiMsgLen);
    m_tcpSocket.read((char*)pdu+sizeof(uint), uiPDULen-sizeof (uint));//读取除了协议总长度意外剩余的内容
//    pdu->caMsg[pdu->uiMsgLen]='\0';
    qDebug() << "recvMsg uiMsgType:" << pdu->uiMsgType
             <<"uiPDULen:"<<pdu->uiPDULen
             <<"uiPDULen:"<<pdu->uiPDULen
             <<"caData:"<<pdu->caData
             <<"caData+32:"<<pdu->caData+32
             <<"caMsg:"<<pdu->caMsg;
    m_prh->pdu = pdu;
    return pdu;
}

void Client::handleMsg(PDU *pdu)
{
    qDebug() << "recvMsg uiMsgType:" << pdu->uiMsgType
             <<"uiPDULen:"<<pdu->uiPDULen
             <<"uiPDULen:"<<pdu->uiPDULen
             <<"caData:"<<pdu->caData
             <<"caData+32:"<<pdu->caData+32
             <<"caMsg:"<<pdu->caMsg;
    m_prh->pdu = pdu;
    switch(pdu->uiMsgType){
    case ENUM_MSG_TYPE_REGIST_RESPOND:
    {
        m_prh->handleRegist();
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND:
    {
        m_prh->handleLogin();
        break;
    }
    case ENUM_MSG_TYPE_FIND_USER_RESPOND:
    {
        m_prh->handleFindUser();
        break;
    }
    case ENUM_MSG_TYPE_ONLINE_USER_RESPOND:
    {
        m_prh->handleOnlineUser();
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
    {
        m_prh->handleAddFriend();
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
    {
        m_prh->handleAddFriendResend();
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE_RESPOND:
    {
        m_prh->handleAddFriendAgree();
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:
    {
        m_prh->handleFlushFriend();
        break;
    }
    case ENUM_MSG_TYPE_CHAT_REQUEST:
    {
        m_prh->handleChatResend();
        break;
    }
    case ENUM_MSG_TYPE_MKDIR_RESPOND:
    {
        m_prh->handleMkdir();
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:
    {
        m_prh->handleFlusFile();
        break;
    }
    case ENUM_MSG_TYPE_DEL_DIR_RESPOND:
    {
        m_prh->handleDelDir();
        break;
    }
    case ENUM_MSG_TYPE_MV_FILE_RESPOND:
    {
        m_prh->handleMvFile();
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:
    {
        m_prh->handleUploadInit();
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPOND:
    {
        m_prh->handleUploadData();
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:
    {
        QMessageBox::information(this,"提示","文件分享成功");
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:
    {
        m_prh->handleShareFileResend();
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_AGREE_RESPOND:
    {
        m_prh->handleShareFileAgree();
        break;
    }
    default:
        break;
    }
}

QString Client::getRootDir()
{
    return m_strRootDir;
}

//QTcpSocket Client::getSocket()
//{
//    return m
//}

void Client::recvMsg()
{
    qDebug() << "\n\nrecvMsg 接收消息长度： " << m_tcpSocket.bytesAvailable();
    QByteArray data = m_tcpSocket.readAll();
    buffer.append(data);

    while(buffer.size()>=(int)(sizeof(PDU))){//判断够不够一个pdu结构体
        PDU* pdu = (PDU*)buffer.data();
        if(buffer.size()<int(pdu->uiPDULen)){//判断够不够完整包
            break;
        }
        //处理一个完整的包
        handleMsg(pdu);
        //一出处理完的数据
        buffer.remove(0,pdu->uiPDULen);
    }
}

void Client::showConnect()
{
    qDebug() << "连接服务器成功" ;
}

void Client::on_regist_PB_clicked()
{
    QString strName = ui->name_LE->text();
    QString strPwd = ui->pwd_LE->text();

    PDU* pdu = mkPDU(ENUM_MSG_TYPE_REGIST_REQUEST, 0);
    memcpy(pdu->caData, strName.toStdString().c_str(),32);
    memcpy(pdu->caData+32, strPwd.toStdString().c_str(),32);
    sendMsg(pdu);
}

void Client::on_login_PB_clicked()
{
    qDebug() << "on_login_PB_clicked_start";
    QString strName = ui->name_LE->text();
    QString strPwd = ui->pwd_LE->text();
    m_strLoginName = strName;

    qDebug() << "strName:"<< strName
             << "strPwd:" << strPwd;
    if(strName.isEmpty() || strPwd.isEmpty()
            || strName.size()>32 || strPwd.size()>32){
        QMessageBox::critical(this,"登录","用户名或者密码非法");
    }
    PDU* pdu = mkPDU(ENUM_MSG_TYPE_LOGIN_REQUEST, 0);

    memcpy(pdu->caData, strName.toStdString().c_str(),32);
    memcpy(pdu->caData+32, strPwd.toStdString().c_str(),32);
    sendMsg(pdu);
}
void Client::uploadError(const QString &error)
{
    QMessageBox::information(this, "提示", error);
}

void Client::sendPDU(PDU *pdu)
{
    m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
    qDebug() << "send uiMsgType:" << pdu->uiMsgType
             <<"uiPDULen:"<<pdu->uiPDULen
             <<"uiPDULen:"<<pdu->uiPDULen
             <<"caData:"<<pdu->caData
             <<"caData+32:"<<pdu->caData+32
             <<"caMsg:"<<pdu->caMsg;
}
