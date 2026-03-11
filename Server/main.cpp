#include "server.h"

#include <QApplication>
#include "operatedb.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OperateDB::getInstance().connect();
    Server::getInstance();
    return a.exec();
}
