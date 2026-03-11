#ifndef REQHANDLER_H
#define REQHANDLER_H

#include "protocol.h"

#include <QFile>
#include <QObject>

class ReqHandler : public QObject
{
    Q_OBJECT
public:
    explicit ReqHandler(QObject *parent = nullptr);
    PDU* pdu;
    qint64 m_iUploadFileSize;
    qint64 m_iReceived;
    QFile m_fUploadFile;
    PDU* handleRegist();
    PDU* handleLogin(QString& loginName);
    PDU* handleFindUser();
    PDU* handleOnlineUser();
    PDU* handleAddFriend();
    PDU* handleAddFriendAgree();
    PDU* handleFlushFriend();
    PDU* handleChat();
    PDU* handleMkdir();
    PDU* handleFlushFile();
    PDU* handleDelDir();
    PDU* handleMvFile();
    PDU* handleUploadInit();
    PDU* handleUploadFileData();
    PDU* handleShareFile();
    PDU* handleShareFileAgree();

signals:

};

#endif // REQHANDLER_H
