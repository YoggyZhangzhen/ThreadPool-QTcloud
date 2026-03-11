#ifndef ONLINEUSER_H
#define ONLINEUSER_H

#include <QDialog>
#include <QListWidgetItem>
namespace Ui {
class OnlineUser;
}

class OnlineUser : public QDialog
{
    Q_OBJECT

public:
    explicit OnlineUser(QWidget *parent = nullptr);
    ~OnlineUser();
    void updateListWidget(QStringList nameList);

private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::OnlineUser *ui;
};

#endif // ONLINEUSER_H
