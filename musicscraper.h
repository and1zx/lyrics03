#ifndef MUSICSCRAPER_H
#define MUSICSCRAPER_H

#include <QObject>
#include <QNetworkAccessManager>

class MusicScraper : public QObject
{
    Q_OBJECT
public:
    explicit MusicScraper(QObject *parent = nullptr);

    void scrape(const QString &url);
    void scrapeList(const QString &title);
    void scrapeDetail(const QString &music_id);
    void downloadMusic(const QString &url,const QString &fileName);
    QString m_mp3Path;
    QString m_lrcPath;
signals:
    void sendPath(const QString &mp3Path,const QString &lrcPath);

private:
    QNetworkAccessManager *m_manager;

    void parseList(QNetworkReply *reply);
    void parseDetail(QNetworkReply *reply);
    bool parseMusicInfo(const QByteArray &data,QString
                                                    &title,QString &artist,QString &mp3Url,QString &lrcUrl);
    void handleNetworkReply(QNetworkReply *reply);
};

#endif // MUSICSCRAPER_H
