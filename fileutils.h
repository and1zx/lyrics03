#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QObject>

class FileUtils : public QObject
{
public:
    explicit FileUtils(QObject *parent = nullptr);
    static QString getFilePath(const QString &fileName);
    static bool exists(const QString &fileName);


signals:

};

#endif // FILEUTILS_H
