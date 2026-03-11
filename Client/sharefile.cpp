#include "client.h"
#include "index.h"
#include "sharefile.h"
#include "ui_sharefile.h"
#include<QDebug>
#include <QMessageBox>

ShareFile::ShareFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShareFile)
{
    ui->setupUi(this);
}

ShareFile::~ShareFile()
{
    delete ui;
}

void ShareFile::updateFriend_LW()
{
    qDebug() << "ui->listWidget is " << (ui->listWidget ? "valid" : "invalid");

    ui->listWidget->clear();
    QListWidget* friend_LW=Index::getInstance().getFriend()->getFriend_LW();
    for(int i=0;i<friend_LW->count();i++){
        QListWidgetItem* friendItem=friend_LW->item(i);
        QListWidgetItem* newItem=new QListWidgetItem(*friendItem);
        ui->listWidget->addItem(newItem);
    }
}

void ShareFile::on_allSelected_PB_clicked()
{
    for(int i=0;i<ui->listWidget->count();i++){
        ui->listWidget->item(i)->setSelected(true);
    }
}

void ShareFile::on_ok_PB_clicked()
{
    //取出好友，计算数量
    QList<QListWidgetItem*>pItems=ui->listWidget->selectedItems();
    //取出分享文件的路径
    QString strPath=Index::getInstance().getFile()->m_strSharedFilePath;
    int friendNum=pItems.size();

    PDU*pdu=mkPDU(ENUM_MSG_TYPE_SHARE_FILE_REQUEST,friendNum*32+strPath.toStdString().size()+1);
    memcpy(pdu->caData, Client::getInstance().m_strLoginName.toStdString().c_str(),32);

    memcpy(pdu->caData+32,&friendNum,sizeof(int));

    for(int i=0;i<friendNum;i++){
        memcpy(pdu->caMsg+i*32,pItems.at(i)->text().toStdString().c_str(),32);
    }
    memcpy(pdu->caMsg+friendNum*32,strPath.toStdString().c_str(),strPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void ShareFile::on_cancleSelected_PB_clicked()
{
    for(int i=0;i<ui->listWidget->count();i++){
        ui->listWidget->item(i)->setSelected(false);
    }
}
