#include "fileutils.h"
#include <QStandardPaths>
#include <QFile>
#include <QDebug>
FileUtils::FileUtils(QObject *parent)
    : QObject{parent}
{

}

QString FileUtils::getFilePath(const QString &fileName)
{
    QString downloadDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/mp3/";
    QString filePath = downloadDir + fileName;
    return filePath;
}

bool FileUtils::exists(const QString &fileName)
{
    QString path = FileUtils::getFilePath (fileName);
    QFile file(path);
    bool isOk = file.exists ();
    qDebug() << path << isOk;

    file.close ();
    return isOk;
}
