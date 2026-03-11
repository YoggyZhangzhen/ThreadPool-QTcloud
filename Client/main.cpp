#include "client.h"
#include "protocol.h"
#include "index.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Index i;
//    i.show();

    qDebug() << sizeof(PDU);
    Client::getInstance().show();
    return a.exec();
}
