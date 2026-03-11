#ifndef UPLOADER_H
#define UPLOADER_H

#include "protocol.h"

#include <QObject>

class Uploader : public QObject
{
    Q_OBJECT
public:
    explicit Uploader(QObject *parent = nullptr);
    Uploader(const QString& filePath);
    QString m_strUploadFilePath;
    void start();
public slots:
    void uploadFile();
signals:
    void errorHandle(const QString& error);
    void uploadPDU(PDU*pdu);
    void finished();
};

#endif // UPLOADER_H
