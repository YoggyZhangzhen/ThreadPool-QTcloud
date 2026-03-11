#ifndef INDEX_H
#define INDEX_H

#include "file.h"
#include "friend.h"

#include <QDialog>

namespace Ui {
class Index;
}

class Index : public QDialog
{
    Q_OBJECT

public:
    ~Index();
    Friend* getFriend();
    File* getFile();
    static Index& getInstance();

private slots:


    void on_friend_PB_clicked();

    void on_file_PB_clicked();

private:
    Ui::Index *ui;
    explicit Index(QWidget *parent = nullptr);
};

#endif // INDEX_H
