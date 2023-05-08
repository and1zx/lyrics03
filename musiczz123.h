#ifndef MUSICZZ123_H
#define MUSICZZ123_H

#include <QObject>
#include <QNetworkAccessManager>
struct Item
{
    QString title;
    QString artist;
    QString id;
    QString fileName;
};
class MusicZz123 : public QObject
{
    Q_OBJECT
public:
    explicit MusicZz123(QObject *parent = nullptr);

    void sendSongInfo(const QString &id);
    void sendSongLocation(const QUrl &url);


    void save(const QString &fileName,const QByteArray &data);

    void search(const QString &keyword);
signals:
    void sendList(QList<Item> items);
    void sendSaved(const QString &songName,const QString &singerName);
private slots:
    void onSongInfoReceived(QNetworkReply *reply);
    void onSongLocationReceived(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_manager;
    QString m_songId;
    QString m_songName;
    QString m_singerName;

    QList<Item> items;

    QMap<QString,QString> mp3Map;
    void handleNetworkReply(QNetworkReply *reply);
    void parseList(QNetworkReply *reply);

};

#endif // MUSICZZ123_H
