#include "db.h"
#include <QMapIterator>
#include <QDateTime>

DB &DB::getInstance()
{
    static DB db;
    return db;
}

DB::DB()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString dbname = path + "/music.db";
    qDebug() << dbname;
    m_db = QSqlDatabase::addDatabase ("QSQLITE");
    m_db.setDatabaseName (dbname);
    if (!m_db.open ()) {
        qDebug() << "打开db 失败" << m_db.lastError ().text ();
    }
    QSqlQuery query(m_db);
    const QString sql = "CREATE TABLE IF NOT EXISTS music ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "title TEXT NOT NULL, "
                        "artist TEXT NOT NULL, "
                        "album TEXT NOT NULL DEFAULT '', "
                        "genre TEXT NOT NULL DEFAULT '', "
                        "year INTEGER DEFAULT 0, "
                        "duration INTEGER DEFAULT 0, "
                        "insert_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                        "update_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                        "source VARCHAR(255) NOT NULL DEFAULT '',"
                        "url TEXT NOT NULL DEFAULT '',"
                        "mid TEXT NOT NULL DEFAULT '',"
                        "is_exists INTEGER DEFAULT 0,"
                        "UNIQUE(title, artist,mid,source))";
    if (!query.exec(sql)) {
        qDebug() << "创建music表失败" << query.lastError().text();
    }
}

bool DB::isOpen() const
{
    return m_db.isOpen ();
}

QList<QHash<QString, QVariant>> DB::search(const QString &keyword)
{
    QList<QHash<QString, QVariant>> result;
    if (!isOpen ()) {
        return result;
    }
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM music WHERE title LIKE :keyword OR artist LIKE :keyword");
    query.bindValue(":keyword", "%" + keyword + "%");
    if (!query.exec ()) {
        qDebug() << "不能搜索从 music 表中" << query.lastError ().text ();
        return result;
    }
    while (query.next ()) {
        QHash<QString, QVariant> item;
        item["id"] = query.value ("id");
        item["title"] = query.value("title");
        item["artist"] = query.value("artist");
        result.append (item);
    }
    return result;
}

bool DB::insert(const QString &title, const QString &artist, const QString &mid,const QString &source)
{
    QSqlQuery query(m_db);
    QString sql = "INSERT INTO music(title, artist, source, mid,insert_time) VALUES('%1', '%2', '%3', '%4',CURRENT_TIMESTAMP)";
    sql = sql.arg(title).arg(artist).arg(source).arg(mid);

    qDebug() << sql;
    bool isOk = query.exec ();

    if (!isOk) {
        qDebug() << "插入失败:"  << m_db.lastError ().text ();
    }else if(query.numRowsAffected() == 0) {
        isOk = false;
        qDebug() << "插入失败:记录已存在";
    }
    return isOk;
}

QString DB::replaceBoundValues(const QSqlQuery& query) {
    QString sql = query.lastQuery();
    QVariantList values = query.boundValues();
    QVariantMap map;
    for (const auto& v : values) {
        map.insert(v.toMap());
    }

    QVariantMap::const_iterator i(map.constBegin());
    while (i != map.constEnd()) {
        const QString key(i.key());
        const QString value(i.value().toString());
        sql.replace(key, value);
        ++i;
    }
    return sql;
}
