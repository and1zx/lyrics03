#ifndef DB_H
#define DB_H

#include <QObject>
#include <QtSql>


class DB
{
public:
    static DB& getInstance();
    DB();
    bool isOpen() const;
    QList<QHash<QString,QVariant>> search(const QString& keyword);

    bool insert(const QString& title,
                const QString& artist,
                const QString& mid,
                const QString &source);
private:
    QSqlDatabase m_db;
    QString replaceBoundValues(const QSqlQuery& query);
};

#endif // DB_H
