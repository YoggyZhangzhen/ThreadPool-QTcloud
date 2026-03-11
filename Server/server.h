#ifndef SERVER_H
#define SERVER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

class Server : public QWidget
{
    Q_OBJECT

public:
    static Server& getInstance();
    void loadConfig();
    ~Server();
    QString getRootDir();

private:
    Server(QWidget *parent = nullptr);
    Ui::Server *ui;
    QString m_strIP;
    quint16 m_usPort;
    QString m_strRootDir;
};
#endif // SERVER_H
