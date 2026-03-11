#include "operatedb.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

OperateDB &OperateDB::getInstance()
{
    static OperateDB instance;
    return instance;
}

void OperateDB::connect()
{
    m_db.setHostName("localhost");
    m_db.setPort(3307);
    m_db.setUserName("root");
    m_db.setPassword("123456");
    m_db.setDatabaseName("mydb2401");
    if(m_db.open()){
        qDebug() << "数据库连接成功";
    }else{
        qDebug() << "数据库连接失败" << m_db.lastError().text();
    }
}

OperateDB::~OperateDB()
{
    m_db.close();
}

bool OperateDB::handleRegist(const char *name, const char *pwd)
{
    if(name == NULL || pwd == NULL){
        return false;
    }
    QString sql = QString("select * from user_info where name='%1'").arg(name);
    qDebug() << "判断要添加的用户是否存在" << sql;
    QSqlQuery q;
    //判断要添加的用户是否存在
    qDebug() << q.exec("select * from user_info;");
    qDebug() << "数据库连接失败" << q.lastError().text();
    if(!q.exec(sql)){//q.next没有说明没有注册
        qDebug() << "123";
        return false;
    }
    if(q.next()){//q.next没有说明没有注册
        qDebug() << "456";
        return false;
    }
    //添加用户
    sql = QString("insert into user_info(name,pwd) values('%1','%2')").arg(name).arg(pwd);
    qDebug() << "添加用户" ;
    return q.exec(sql);
}

bool OperateDB::handleLogin(const char *name, const char *pwd)
{
    if(name == NULL || pwd == NULL){
        return false;
    }
    //检查登录的用户密码是否正确
    QString sql = QString("select * from user_info where name='%1' and pwd='%2'").arg(name).arg(pwd);
    qDebug() << "判断要添加的用户是否存在" << sql;
    QSqlQuery q;
    if(!q.exec(sql) || !q.next()){
        qDebug() << "";
        return false;
    }
    sql = QString("update user_info set online=1 where name='%1' and pwd='%2'").arg(name).arg(pwd);
    qDebug() << "online置1" << sql;
    return q.exec(sql);
}

void OperateDB::handleOffline(const char *name)
{
    if(name == NULL){
        qDebug() << "handleOffline name is null";
        return;
    }
    QString sql = QString("update user_info set online=0 where name='%1'").arg(name);
    qDebug() << "online置0" << sql;
    QSqlQuery q;
    q.exec(sql);
}

int OperateDB::handleFindUser(const char *name)
{//-1失败，0不在线，1在线，2不存在
    if(name == NULL){
        qDebug() << "handleOffline name is null";
        return -1;
    }
    QString sql = QString("select online from user_info where name='%1'").arg(name);
    qDebug() << "按用户名查找用户" << sql;
    QSqlQuery q;
    if(!q.exec(sql)){
        return -1;
    }
    if(q.next()){
        return q.value(0).toInt();
    }
    return 2;
}

QStringList OperateDB::handleOnlineUser()
{
    QString sql = QString("select name from user_info where online=1");
    qDebug() << "查找在线用户名" << sql;
    QSqlQuery q;
    q.exec(sql);

    QStringList res;
    res.clear();
    while(q.next()){
        res.append(q.value(0).toString());
    }
    return res;
}

int OperateDB::handleAddFriend(const char *curName, const char *tarName)
{
    if(curName == NULL || tarName == NULL){
        return -1;
    }

    QString sql = QString(R"(
                          select * from friend where
                          (
                          user_id=(select id from user_info where name='%1')
                          and
                          friend_id=(select id from user_info where name='%2')
                          )
                          or
                          (
                          user_id=(select id from user_info where name='%2')
                          and
                          friend_id=(select id from user_info where name='%1')
                          )
                          )").arg(curName).arg(tarName);
    qDebug() << "是否已是好友" <<sql;
    QSqlQuery q;
    q.exec(sql);
    if(q.next()){
        return -2;
    }
    sql = QString("select online from user_info where name='%1'").arg(tarName);
    qDebug() << "是否在线" <<sql;
    q.exec(sql);
    if(q.next()){
        return q.value(0).toInt();
    }
    return -1;
}

bool OperateDB::handleAddFriendAgree(const char *curName, const char *tarName)
{
    if(curName == NULL || tarName == NULL){
        return -1;
    }

    QString sql = QString(R"(
                          insert into friend(user_id,friend_id)
                          select u1.id,u2.id from user_info u1,user_info u2
                          where u1.name='%1' and u2.name='%2'
                          )").arg(curName).arg(tarName);
    qDebug() << "插入好友" <<sql;
    QSqlQuery q;
    return q.exec(sql);
}

QStringList OperateDB::handleFlushFriend(const char *name)
{
    QString sql = QString(R"(
                          select name from user_info where online=1 and id in
                          (
                          select friend_id from friend where user_id = (select id from user_info where name = '%1')
                          union
                          select user_id from friend where friend_id = (select id from user_info where name = '%1')
                          )
                          )").arg(name);
    qDebug() << "查找好友" << sql;
    QSqlQuery q;
    q.exec(sql);

    QStringList res;
    res.clear();
    while(q.next()){
        res.append(q.value(0).toString());
    }
    return res;
}

OperateDB::OperateDB(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QMYSQL");
}
