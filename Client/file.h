#ifndef FILE_H
#define FILE_H

#include "protocol.h"
#include "sharefile.h"

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class File;
}

class File : public QDialog
{
    Q_OBJECT

public:
    explicit File(QWidget *parent = nullptr);
    ~File();
    QString m_strUserPath;
    QString m_strCurPath;
    void flushFile();
    void updateFileList(QList<FileInfo*>pFileList);
    QList<FileInfo*> m_pFileInfoList;
    bool isDir(QString strFileName);
    QString m_strMoveFileName;
    QString m_strMoveFilePath;
    QString m_strUploadFilePath;
    QString m_strSharedFilePath;
    ShareFile* m_pShareFile;
    void uploadFile();


private slots:
    void on_mkdir_PB_clicked();

    void on_flushFile_PB_clicked();

    void on_rmdir_PB_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);


    void on_return_PB_clicked();

    void on_mv_PB_clicked();

    void on_upload_PB_clicked();

    void on_share_PB_clicked();

private:
    Ui::File *ui;
};

#endif // FILE_H
