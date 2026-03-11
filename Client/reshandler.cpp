#include "client.h"
#include "index.h"
#include "reshandler.h"
#include "uploader.h"

#include <QMessageBox>

ResHandler::ResHandler(QObject *parent) : QObject(parent)
{

}

void ResHandler::handleRegist()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret){
        QMessageBox::information(&Client::getInstance(), "注册", "注册成功");
    }else{
        QMessageBox::information(&Client::getInstance(), "注册", "注册失败");
    }
}

void ResHandler::handleLogin()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret){
        Index::getInstance().show();
        Client::getInstance().hide();//隐藏当前登录页面
    }else{
        QMessageBox::information(&Client::getInstance(), "登录", "登录失败");
    }
}

void ResHandler::handleFindUser()
{
    int ret;
    char caName[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    memcpy(&ret, pdu->caData+32, sizeof(int));
    Friend* f = Index::getInstance().getFriend();
    if(ret == -1){
        QMessageBox::information(f, "提示", "查找失败");//第一个参数是弹出的页面
    }else if(ret == 0){
        QMessageBox::information(f, "提示", "该用户离线");
    }else if(ret == 1){
        int ret = QMessageBox::information(f, "提示", "该用户在线", "添加好友","取消");
        if(ret==0){
            QString strCurName = Client::getInstance().m_strLoginName;
            QString strTarName = caName;

            PDU* pdu = mkPDU(ENUM_MSG_TYPE_ADD_FRIEND_REQUEST, 0);

            memcpy(pdu->caData, strCurName.toStdString().c_str(),32);
            memcpy(pdu->caData+32, strTarName.toStdString().c_str(),32);
            Client::getInstance().sendMsg(pdu);
        }
    }else if(ret == 2){
        QMessageBox::information(f, "提示", "该用户不存在");
    }
}

void ResHandler::handleOnlineUser()
{
    uint uiSize = pdu->uiMsgLen/32;
    QStringList nameList;
    char tmp[32];
    for(uint i = 0;i < uiSize;i++){
        memcpy(tmp, pdu->caMsg+i*32, 32);
        if(QString(tmp) == Client::getInstance().m_strLoginName){
            continue;
        }
        nameList.append(tmp);
    }
    Index::getInstance().getFriend()->m_pOnlineUser->updateListWidget(nameList);
}

void ResHandler::handleAddFriend()
{
    int ret;
    memcpy(&ret, pdu->caData, sizeof(int));
    Friend* f = Index::getInstance().getFriend();
    if(ret == -1){
        QMessageBox::information(f, "提示", "添加失败");//第一个参数是弹出的页面
    }else if(ret == 0){
        QMessageBox::information(f, "提示", "该用户离线");
    }else if(ret == -2){
        QMessageBox::information(f, "提示", "对方已经是你的好友");
    }
}

void ResHandler::handleAddFriendResend()
{
    char caName[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    int ret = QMessageBox::question(Index::getInstance().getFriend(), "添加好友", QString("是否同意%1的添加好友请求？").arg(caName));
    if(ret != QMessageBox::Yes){
        return;
    }
    PDU* respdu = mkPDU(ENUM_MSG_TYPE_ADD_FRIEND_AGREE_REQUEST, 0);
    memcpy(respdu->caData, pdu->caData, 64);
    Client::getInstance().sendMsg(respdu);
}

void ResHandler::handleAddFriendAgree()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret){
        QMessageBox::information(Index::getInstance().getFriend(), "提示", "添加好友成功");
        Index::getInstance().getFriend()->flushFriend();
    }else{
        QMessageBox::information(Index::getInstance().getFriend(), "提示", "添加好友失败");
    }
}

void ResHandler::handleFlushFriend()
{
    uint uiSize = pdu->uiMsgLen/32;
    QStringList nameList;
    char tmp[32];
    for(uint i = 0;i < uiSize;i++){
        memcpy(tmp, pdu->caMsg+i*32, 32);
        nameList.append(tmp);
    }
    Index::getInstance().getFriend()->updateListWidget(nameList);
}

void ResHandler::handleChatResend()
{
    char caChatName[32] = {'\0'};
    memcpy(caChatName, pdu->caData,32);
    Chat*c = Index::getInstance().getFriend()->m_pChat;
    if(c->isHidden()){
        c->setWindowTitle(caChatName);
        c->show();
    }
    c->m_strChatName=caChatName;
    c->update_TE(QString("%1:%2").arg(caChatName).arg(pdu->caMsg));
}

void ResHandler::handleMkdir()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret){
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(&Client::getInstance(), "提示", "创建文件夹失败");
    }
}

void ResHandler::handleFlusFile()
{
    int iCount = pdu->uiMsgLen/sizeof(FileInfo);
    QList<FileInfo*>pFileList;
    for(int i=0;i<iCount;i++){
        FileInfo* pFileInfo=new FileInfo;
        memcpy(pFileInfo,pdu->caMsg+i*sizeof(FileInfo),sizeof(FileInfo));
        pFileList.append(pFileInfo);
    }
    Index::getInstance().getFile()->updateFileList(pFileList);
}

void ResHandler::handleDelDir()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret){
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(&Client::getInstance(), "提示", "删除文件夹失败");
    }
}

void ResHandler::handleMvFile()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret){
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(&Client::getInstance(), "提示", "移动文件夹失败");
    }
}

void ResHandler::handleUploadInit()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret){
        Uploader *uploader=new Uploader(Index::getInstance().getFile()->m_strUploadFilePath);
        connect(uploader,&Uploader::errorHandle,&Client::getInstance(),&Client::uploadError);
        connect(uploader,&Uploader::uploadPDU,&Client::getInstance(),&Client::sendMsg);
        uploader->start();

    }else{
        QMessageBox::information(&Client::getInstance(), "提示", "上传文件失败");
    }
}

void ResHandler::handleUploadData()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret){
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(&Client::getInstance(), "提示", "上传文件失败");
    }
}

void ResHandler::handleShareFile()
{

}

void ResHandler::handleShareFileResend()
{
     QString strPath =QString(pdu->caMsg);
     int index=strPath.lastIndexOf('/');
     QString strFileName=strPath.right(strPath.size()-index-1);

     QString strMsg = QString("%1 分享文件 %2\n是否接受？").arg(pdu->caData).arg(strFileName);
     int ret = QMessageBox::question(&Index::getInstance(),"分享文件",strMsg);
     if(ret!=QMessageBox::Yes){
         return;
     }
     PDU*respdu=mkPDU(ENUM_MSG_TYPE_SHARE_FILE_AGREE_REQUEST,pdu->uiMsgLen);
     memcpy(respdu->caData,Client::getInstance().m_strLoginName.toStdString().c_str(),32);
     memcpy(respdu->caMsg,pdu->caMsg,pdu->uiMsgLen);
     Client::getInstance().sendMsg(respdu);
}

void ResHandler::handleShareFileAgree()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret){
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(&Client::getInstance(), "提示", "分享文件失败");
    }
}


