#include "client.h"
#include "onlineuser.h"
#include "ui_onlineuser.h"

OnlineUser::OnlineUser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OnlineUser)
{
    ui->setupUi(this);
}

OnlineUser::~OnlineUser()
{
    delete ui;
}

void OnlineUser::updateListWidget(QStringList nameList)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(nameList);
}

void OnlineUser::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString strCurName = Client::getInstance().m_strLoginName;
    QString strTarName = item->text();

    PDU* pdu = mkPDU(ENUM_MSG_TYPE_ADD_FRIEND_REQUEST, 0);

    memcpy(pdu->caData, strCurName.toStdString().c_str(),32);
    memcpy(pdu->caData+32, strTarName.toStdString().c_str(),32);
    Client::getInstance().sendMsg(pdu);
}
