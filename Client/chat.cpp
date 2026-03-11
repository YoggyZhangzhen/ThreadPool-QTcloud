#include "chat.h"
#include "client.h"
#include "protocol.h"
#include "ui_chat.h"
#include "string.h"


Chat::Chat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
}

Chat::~Chat()
{
    delete ui;
}

void Chat::update_TE(QString strMsg)
{
    ui->show_TE->append(strMsg);
}

void Chat::on_send_PB_clicked()
{
    QString strMsg = ui->input_LE->text();
    if(strMsg.isEmpty()){
        return;
    }
    PDU*pdu = mkPDU(ENUM_MSG_TYPE_CHAT_REQUEST, strMsg.toStdString().size()+1);
    memcpy(pdu->caData, Client::getInstance().m_strLoginName.toStdString().c_str(), 32);
    memcpy(pdu->caData+32, m_strChatName.toStdString().c_str(), 32);
    memcpy(pdu->caMsg, strMsg.toStdString().c_str(), strMsg.toStdString().size());
    Client::getInstance().sendMsg(pdu);
    ui->input_LE->clear();
}
