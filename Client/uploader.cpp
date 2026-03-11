#include "uploader.h"

#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QThread>

Uploader::Uploader(QObject *parent) : QObject(parent)
{

}

void Uploader::uploadFile()
{
    //打开要上传文件
      QFile file(m_strUploadFilePath);
      if(!file.open(QIODevice::ReadOnly)){
          emit errorHandle("打开文件夹失败");
          emit finished();
          return;
      }


      //循环发送
      while(true){
          //构建pdu，每次发送4096
          PDU* pdu=mkPDU(ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUEST,4096);
          int ret=file.read(pdu->caMsg,4096);
          if(ret==0){
              break;
          }
          if(ret<0){
              emit errorHandle("读取文件失败");
              break;
          }
          //更新长度并且发送
          pdu->uiMsgLen = ret;
          pdu->uiPDULen = ret + sizeof(PDU);
          emit uploadPDU(pdu);
//          Client::getInstance().m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
//              qDebug() << "send uiMsgType:" << pdu->uiMsgType
//                       <<"uiPDULen:"<<pdu->uiPDULen
//                       <<"uiPDULen:"<<pdu->uiPDULen
//                       <<"caData:"<<pdu->caData
//                       <<"caData+32:"<<pdu->caData+32
//                       <<"caMsg:"<<pdu->caMsg;

      }
      file.close();
      emit finished();

}

Uploader::Uploader(const QString &filePath): m_strUploadFilePath(filePath)
{

}

void Uploader::start()
{
    QThread* thread = new QThread;
    this->moveToThread(thread);

    connect(thread,&QThread::started,this,&Uploader::uploadFile);
    connect(this,&Uploader::finished,thread,&QThread::quit);

    connect(thread,&QThread::finished,thread,&QThread::deleteLater);
    thread->start();
}
