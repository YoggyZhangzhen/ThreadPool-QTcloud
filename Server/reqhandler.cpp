#include "reqhandler.h"
#include <QDebug>
#include <QDir>
#include "mytcpserver.h"
#include "operatedb.h"
#include "server.h"

ReqHandler::ReqHandler(QObject *parent) : QObject(parent)
{

}

PDU *ReqHandler::handleRegist()
{
    qDebug() << "ENUM_MSG_TYPE_REGIST_REQUEST";
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};

    memcpy(caName, pdu->caData, 32);
    memcpy(caPwd, pdu->caData+32, 32);
    qDebug() << "caName:" << caName
             << "caPwd:" << caPwd;

    bool ret = OperateDB::getInstance().handleRegist(caName, caPwd);
    qDebug() << "ret:" << ret;
    if(ret){
        QDir dir;
        bool boolSuccess = dir.mkdir(QString("%1/%2").arg(Server::getInstance().getRootDir()).arg(caName));
        qDebug() << "mkdir boolSuccess:" << boolSuccess;
    }

    PDU* respdu = mkPDU(ENUM_MSG_TYPE_REGIST_RESPOND, 0);
    memcpy(respdu->caData, &ret, sizeof(ret));
    return respdu;
}

PDU *ReqHandler::handleLogin(QString &loginName)
{
    qDebug() << "ENUM_MSG_TYPE_LOGIN_REQUEST";
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};

    memcpy(caName, pdu->caData, 32);
    memcpy(caPwd, pdu->caData+32, 32);
    qDebug() << "caName:" << caName
             << "caPwd:" << caPwd;

    bool ret = OperateDB::getInstance().handleLogin(caName, caPwd);
    loginName = caName;
    qDebug() << "ret:" << ret;

    PDU* respdu = mkPDU(ENUM_MSG_TYPE_LOGIN_RESPOND, 0);
    memcpy(respdu->caData, &ret, sizeof(ret));
    return respdu;
}

PDU *ReqHandler::handleFindUser()
{
    char caName[32] = {'\0'};

    memcpy(caName, pdu->caData, 32);
    qDebug() << "find user caName:" << caName;

    int ret = OperateDB::getInstance().handleFindUser(caName);
    qDebug() << "find user ret:" << ret;

    PDU* respdu = mkPDU(ENUM_MSG_TYPE_FIND_USER_RESPOND, 0);
    memcpy(respdu->caData, pdu->caData, 32);
    memcpy(respdu->caData+32, &ret, sizeof(ret));
    return respdu;
}

PDU *ReqHandler::handleOnlineUser()
{
    qDebug() << "online user start";

    QStringList res = OperateDB::getInstance().handleOnlineUser();
    qDebug() << "online user ret.size:" << res.size();

    PDU* respdu = mkPDU(ENUM_MSG_TYPE_ONLINE_USER_RESPOND, res.size()*32);
    for(int i = 0;i < res.size();i++){
        qDebug() << "res.at(i):"<< res.at(i);
        memcpy(respdu->caMsg + i*32, res.at(i).toStdString().c_str(), 32);
    }
    return respdu;
}

PDU *ReqHandler::handleAddFriend()
{
    char caCurName[32] = {'\0'};
    char caTarName[32] = {'\0'};

    memcpy(caCurName, pdu->caData, 32);
    memcpy(caTarName, pdu->caData+32, 32);
    qDebug() << "handleAddFriend caCurName:" << caCurName
             << "caTarName:" << caTarName;

    int ret = OperateDB::getInstance().handleAddFriend(caCurName, caTarName);
    qDebug() << "handleAddFriend ret:" << ret;

    if(ret==1){
        //转发给目标客户端
        MyTcpServer::getInstance().resend(caTarName, pdu);
    }

    PDU* respdu = mkPDU(ENUM_MSG_TYPE_ADD_FRIEND_RESPOND, 0);
    memcpy(respdu->caData, &ret, sizeof(int));
    return respdu;
}

PDU *ReqHandler::handleAddFriendAgree()
{
    char caCurName[32] = {'\0'};
    char caTarName[32] = {'\0'};

    memcpy(caCurName, pdu->caData, 32);
    memcpy(caTarName, pdu->caData+32, 32);
    qDebug() << "handleAddFriendAgree caCurName:" << caCurName
             << "caTarName:" << caTarName;

    bool ret = OperateDB::getInstance().handleAddFriendAgree(caCurName, caTarName);
    qDebug() << "handleAddFriendAgree ret:" << ret;

    if(ret==1){
        //转发给目标客户端
        MyTcpServer::getInstance().resend(caTarName, pdu);
    }

    PDU* respdu = mkPDU(ENUM_MSG_TYPE_ADD_FRIEND_AGREE_RESPOND, 0);
    memcpy(respdu->caData, &ret, sizeof(bool));
    return respdu;
}

PDU *ReqHandler::handleFlushFriend()
{
    char caName[32] = {'\0'};

    memcpy(caName, pdu->caData, 32);
    qDebug() << "handleFlushFriend caName:" << caName;

    QStringList res = OperateDB::getInstance().handleFlushFriend(caName);
    qDebug() << "friend ret.size:" << res.size();

    PDU* respdu = mkPDU(ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND, res.size()*32);
    for(int i = 0;i < res.size();i++){
        qDebug() << "res.at(i):"<< res.at(i);
        memcpy(respdu->caMsg + i*32, res.at(i).toStdString().c_str(), 32);
    }
    return respdu;
}

PDU *ReqHandler::handleChat()
{
    char caTarName[32] = {'\0'};
    memcpy(caTarName, pdu->caData+32, 32);
    qDebug() << "handleChat caTarName:" << caTarName;
    MyTcpServer::getInstance().resend(caTarName, pdu);
    return NULL;
}

PDU *ReqHandler::handleMkdir()
{
    QString strCurPath = pdu->caMsg;
    char caDirName[32] = {'\0'};
    memcpy(caDirName, pdu->caData,32);
    QString strNewDir = QString("%1/%2").arg(strCurPath).arg(caDirName);
    qDebug() << "handleMkdir strNewDir:" << strNewDir;
    QDir dir;
    bool ret =true;
    if(dir.exists(strNewDir)){
        ret=false;
        qDebug()<<"dir.exists";
    }else{
        ret=dir.mkdir(strNewDir);
    }
    qDebug()<<"handleMkdir ret"<<ret;
    PDU* respdu = mkPDU(ENUM_MSG_TYPE_MKDIR_RESPOND, 0);
    memcpy(respdu->caData, &ret, sizeof(ret));
    return respdu;
}

PDU *ReqHandler::handleFlushFile()
{
    //获取当前路径下的所有文件
    QDir dir(pdu->caMsg);
    QFileInfoList fileInfoList = dir.entryInfoList();

    int iFileCount = fileInfoList.size()-2;
    //构建pdu,文件以结构体形式放入caMsg
    PDU* respdu = mkPDU(ENUM_MSG_TYPE_FLUSH_FILE_RESPOND, sizeof(FileInfo)*iFileCount);

    //遍历文件，将每个文件的文件名和类型放入caMsg
    FileInfo* pFileInfo = NULL;
    QString strFileName;
    for(int i=0,j=0;i<fileInfoList.size();i++){
        strFileName = fileInfoList[i].fileName();
        if(strFileName==QString(".")||strFileName==QString("..")){
            continue;
        }
        pFileInfo = (FileInfo*)respdu->caMsg+j++;
        memcpy(pFileInfo->caName,strFileName.toStdString().c_str(),32);//caMsg转成FileInfo类型进行偏移

        if(fileInfoList[i].isDir()){
            pFileInfo->iFileType=0;
        }else if(fileInfoList[i].isFile()){
            pFileInfo->iFileType=1;
        }
        qDebug()<<"handleFlushFile strFileName"<<strFileName<<"iFileType"<<pFileInfo->iFileType;
    }
    qDebug()<<"handleFlushFile fileInfoList.size()"<<fileInfoList.size();
    return respdu;
}

PDU *ReqHandler::handleDelDir()
{
    QDir dir(pdu->caMsg);
    bool ret = dir.removeRecursively();
    PDU* respdu = mkPDU(ENUM_MSG_TYPE_DEL_DIR_RESPOND,0);
    memcpy(respdu->caData,&ret,sizeof (ret));
    return respdu;
}

PDU *ReqHandler::handleMvFile()
{
    int srcLen = 0;
    int tarLen = 0;
    memcpy(&srcLen,pdu->caData,sizeof(int));
    memcpy(&tarLen,pdu->caData+32,sizeof(int));

    char* pSrcPath=new char[srcLen+1];
    char* pTarPath=new char[tarLen+1];
    memset(pSrcPath,'\0',srcLen+1);
    memset(pTarPath,'\0',tarLen+1);
    memcpy(pSrcPath,pdu->caMsg,srcLen);
    memcpy(pTarPath,pdu->caMsg+srcLen,tarLen);


    qDebug() << "pSrcPath"<<pSrcPath
             <<"pTarPath"<<pTarPath;

    bool ret=QFile::rename(pSrcPath,pTarPath);
    PDU*respdu=mkPDU(ENUM_MSG_TYPE_MV_FILE_RESPOND,0);
    memcpy(respdu->caData,&ret,sizeof(ret));
    return respdu;
}

PDU *ReqHandler::handleUploadInit()
{
    //取出pdu文件名，大小，当前路径
    qint64 iFileSize=0;
    char caFileName[32]={'\0'};
    memcpy(caFileName,pdu->caData,32);
    memcpy(&iFileSize,pdu->caData+32,sizeof(qint64));

    //上传文件的完整路径
    QString strFilePath = QString("%1/%2").arg(pdu->caMsg).arg(caFileName);

    //创建文件
    m_fUploadFile.setFileName(strFilePath);
    bool ret=false;

    //创建并打开成功
    if(m_fUploadFile.open(QIODevice::WriteOnly)){
        m_iUploadFileSize=iFileSize;
        m_iReceived=0;
        ret=true;
    }
    //返回结果
    PDU*respdu=mkPDU(ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,0);
    memcpy(respdu->caData,&ret,sizeof(ret));
    return respdu;
}

PDU *ReqHandler::handleUploadFileData()
{
    m_fUploadFile.write(pdu->caMsg,pdu->uiMsgLen);
    m_iReceived+=pdu->uiMsgLen;
    if(m_iReceived<m_iUploadFileSize){
        return NULL;
    }
    m_fUploadFile.close();
    PDU*respdu=mkPDU(ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPOND,0);
    bool ret=m_iReceived==m_iUploadFileSize;
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

PDU *ReqHandler::handleShareFile()
{
    //取出好友数量
    int friendNum=0;
    memcpy(&friendNum,pdu->caData+32,sizeof(int));

    PDU*resendpdu=mkPDU(pdu->uiMsgType,pdu->uiMsgLen-friendNum*32);
    memcpy(resendpdu->caData,pdu->caData,32);
    memcpy(resendpdu->caMsg,pdu->caMsg+friendNum*32,pdu->uiMsgLen-friendNum*32);
    //遍历好友进行转发
    char caRecvName[32]={'\0'};
    for(int i=0;i<friendNum;i++){
        memcpy(caRecvName,pdu->caMsg+i*32,32);
        qDebug()<<"caRecvName"<<caRecvName;
        MyTcpServer::getInstance().resend(caRecvName,resendpdu);
    }
    free(resendpdu);
    resendpdu=NULL;
    //响应给当前用户
    return mkPDU(ENUM_MSG_TYPE_SHARE_FILE_RESPOND,0);
}

PDU *ReqHandler::handleShareFileAgree()
{
    QString strRecvPath = QString("%1/%2").arg(Server::getInstance().getRootDir()).arg(pdu->caData);
    QString strSrcPath =QString(pdu->caMsg);
    int index=strSrcPath.lastIndexOf('/');
    QString strFileName=strSrcPath.right(strSrcPath.size()-index-1);
    strRecvPath=strRecvPath+"/"+strFileName;

    bool ret=QFile::copy(strSrcPath,strRecvPath);
    PDU*respdu=mkPDU(ENUM_MSG_TYPE_SHARE_FILE_AGREE_RESPOND,0);
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;

}
