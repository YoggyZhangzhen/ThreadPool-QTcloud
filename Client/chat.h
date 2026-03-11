#ifndef CHAT_H
#define CHAT_H

#include <QDialog>
#include "string.h"

namespace Ui {
class Chat;
}

class Chat : public QDialog
{
    Q_OBJECT

public:
    explicit Chat(QWidget *parent = nullptr);
    ~Chat();
    QString m_strChatName;
    void update_TE(QString strMsg);

private slots:
    void on_send_PB_clicked();

private:
    Ui::Chat *ui;
};

#endif // CHAT_H
