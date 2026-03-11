#include "friend.h"
#include "ui_friend.h"
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include "protocol.h"
#include "client.h"

Friend::Friend(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Friend)
{
    ui->setupUi(this);
    m_pOnlineUser = new OnlineUser;
    m_pChat = new Chat;
    flushFriend();
}

Friend::~Friend()
{
    delete m_pChat;
    delete m_pOnlineUser;
    delete ui;
}

void Friend::updateListWidget(QStringList nameList)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(nameList);
}

void Friend::flushFriend()
{
    PDU* pdu = mkPDU(ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST, 0);
    memcpy(pdu->caData, Client::getInstance().m_strLoginName.toStdString().c_str(), 32);
    Client::getInstance().sendMsg(pdu);
}

QListWidget *Friend::getFriend_LW()
{
    return ui->listWidget;
}



void Friend::on_findUser_PB_clicked()
{
    QString strName = QInputDialog::getText(this, "查找用户", "用户名：");
    if(strName.isEmpty())
    {
        return;
    }
    qDebug() << "strName:" << strName;
    PDU* pdu = mkPDU(ENUM_MSG_TYPE_FIND_USER_REQUEST, 0);
    memcpy(pdu->caData,strName.toStdString().c_str(), 32);
    Client::getInstance().sendMsg(pdu);
}

void Friend::on_onlineUser_PB_clicked()
{
    if(m_pOnlineUser->isHidden()){
        m_pOnlineUser->show();
    }
    PDU* pdu = mkPDU(ENUM_MSG_TYPE_ONLINE_USER_REQUEST, 0);
    Client::getInstance().sendMsg(pdu);
}

void Friend::on_flush_PB_clicked()
{
    flushFriend();
}

void Friend::on_chat_PB_clicked()
{
    QListWidgetItem*pItem = ui->listWidget->currentItem();
    if(!pItem){
        QMessageBox::information(this,"提示","请选择要聊天的好友");
        return;
    }
    m_pChat->m_strChatName = pItem->text();
    if(m_pChat->isHidden()){
        m_pChat->show();
        m_pChat->setWindowTitle(pItem->text());
    }
}
