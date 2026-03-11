#ifndef CLIENT_H
#define CLIENT_H

#include "protocol.h"
#include "reshandler.h"

#include <QWidget>
#include <QTcpSocket>


QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QWidget
{
    Q_OBJECT

public:
    static Client& getInstance();
    ~Client();
    void loadConfig();
    QString m_strLoginName;
    ResHandler* m_prh;
    PDU* readMsg();
    void handleMsg(PDU* pdu);
    QString getRootDir();
    QByteArray buffer;
//    QTcpSocket getSocket();
    QTcpSocket m_tcpSocket;


public slots:
    void showConnect();
    void recvMsg();
    void on_regist_PB_clicked();
    void uploadError(const QString& error);
    void sendPDU(PDU* pdu);
    void sendMsg(PDU* pdu);


private slots:
    void on_login_PB_clicked();

private:
    Ui::Client *ui;
    QString m_strIP;
    quint16 m_usPort;
    QString m_strRootDir;
    Client(QWidget *parent = nullptr);
    Client(const Client& instance) = delete;
    Client& operator=(const Client&) = delete;
};
#endif // CLIENT_H
