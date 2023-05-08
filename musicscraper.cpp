#include "musicscraper.h"
#include <QNetworkReply>
#include <iostream>
#include <QRegularExpression>
#include <QEventLoop>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <iostream>
MusicScraper::MusicScraper(QObject *parent)
    : QObject{parent},
    m_manager(new QNetworkAccessManager(this))

{
    qDebug() << "MusicScraper";
    connect (m_manager,&QNetworkAccessManager::finished ,this,&MusicScraper::handleNetworkReply );
}

void MusicScraper::scrape(const QString &url)
{
    QUrl qurl(url);
    QNetworkRequest request(qurl);
    m_manager->get (request);
}
//获取搜索
void MusicScraper::scrapeList(const QString &title)
{
//    QString url = QString("https://www.gequbao.com/s/%1").arg (title);
    QString url = QString("https://zz123.com/search/?key=%1").arg (title);
    scrape (url);
}

void MusicScraper::scrapeDetail(const QString &music_id)
{
    QString url = QString("https://www.gequbao.com/music/%1").arg (music_id);
    scrape (url);
}


void MusicScraper::handleNetworkReply(QNetworkReply *reply)
{
    QString url = reply->url ().toString ();
    qDebug() << url << reply->url ().path ();
    if (reply->error () != QNetworkReply::NoError) {
        std::cerr << "Network request failed: " << reply->errorString ().toStdString () << std::endl;
        reply->deleteLater ();
        return;
    }


    QString path = reply->url ().path ();
//    if (path.startsWith ("/s/")) {
    if (path.startsWith ("/search/")) {
        parseList (reply);
    }
    if (path.startsWith ("/music/")) {
        parseDetail (reply);
    }


    reply->deleteLater ();
}

void MusicScraper::downloadMusic(const QString &url, const QString &fileName)
{
//    int blockSize = 64 * 1024; // 每个块的大小为64KB
//    QString downloadDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QString downloadDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/mp3/";

    QString filePath = downloadDir + fileName;

    QDir dir(downloadDir);
    if (!dir.exists ()) {
        dir.mkpath (downloadDir);
    }
    QFile file(filePath);

    if (file.exists ()) {
        qDebug() << "file exists " << filePath;
        return;
    }
    QUrl qurl(url);
    QNetworkRequest request(qurl);
    QNetworkReply *reply = m_manager->get (request);
    QEventLoop loop;
    connect (reply,&QNetworkReply::finished,&loop,&QEventLoop::quit);
//    QObject::connect(reply, &QNetworkReply::finished, [&](){
//        if (reply->error() != QNetworkReply::NoError) {
//            qDebug() << "Error:" << reply->errorString();
//            file.close();
//            file.remove();
//            return;
//        }
//        qDebug() << "Download finished.";
//        file.close();
//    });
//    QObject::connect(reply, &QIODevice::readyRead, [&](){
//        qint64 bytesRead = 0;
//        while (bytesRead < reply->bytesAvailable()) {
//            QByteArray buffer = reply->read(qMin(reply->bytesAvailable() - bytesRead, blockSize));
//            file.write(buffer);
//            bytesRead += buffer.size();
//            file.seek(file.pos() - bytesRead);
//            qDebug() << "Downloaded" << file.pos() / 1024 << "KB...";
//        }
//    });

    loop.exec ();

    QByteArray data =reply->readAll ();
    if (file.open (QIODevice::WriteOnly)) {
        file.write (data);
        std::cout << "Downloaded music to " << filePath.toStdString() << std::endl;
    }else{
        std::cerr << "Failed to open file " << filePath.toStdString() << " for writing" << std::endl;
    }
    file.close ();
}

void MusicScraper::parseList(QNetworkReply *reply)
{
    QByteArray data = reply->readAll ();
//    qDebug().noquote () << "parseList" << QString::fromUtf8 (data);
//    std::cout << QString::fromUtf8 (data).toStdString ().data ();



    QRegularExpression re("class=\"songname[^>]*>[^<]*<a href=\"([^\"]*)\" title=\"([^\"]*)\"[^<]*</a>[^>]*>[^>]*>[^<]*<a href=\"([^\"]*)\" title=\"([^\"]*)\"");

    QRegularExpressionMatchIterator matches = re.globalMatch(QString::fromUtf8 (data));
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        QString playUrl = match.captured(1);
        QString title = match.captured(2);
        QString singerUrl = match.captured(3);
        QString artist = match.captured(4);

        QString id = playUrl.mid(6, playUrl.size() - 10);

//        QString downloadDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/mp3/";
        QString fileName  = title + "-" + artist + ".mp3";

//        QDir dir(downloadDir);
//        if (!dir.exists ()) {
//            dir.mkpath (downloadDir);
//        }


        QString url = QString("https://zz123.com/ajax/?act=songplay&id=%1").arg (id);
        QUrl qurl(url);
        QNetworkAccessManager* manager = new QNetworkAccessManager();
        QNetworkRequest request(qurl);
        QNetworkReply* reply = manager->get(request);
        QObject::connect(reply, &QNetworkReply::redirected, this, [=](const QUrl &url){
            // 处理重定向
            reply->abort();
            qDebug() << url.toString ();
            downloadMusic (url.toString (),fileName);
        });

//        QObject::connect(reply, &QNetworkReply::finished, [=]() {
//            if (reply->attribute(QNetworkRequest::RedirectionTargetAttribute).isValid()) {
//                QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
//                qDebug() << redirectUrl.toString();
//            }
//            else {
//                QFile file(filePath);
//                if (file.exists ()) {
//                    qDebug() << "file exists " << filePath;
//                    return;
//                }
//                if (!file.open (QIODevice::WriteOnly)) {
//                    return;
//                }
//                file.write(reply->readAll ());
//                file.close();


//            }
//            reply->deleteLater();
//            manager->deleteLater();
//        });

        qDebug() << playUrl << id << title << singerUrl << artist;
        break;
    }
    return;
    //解析 <a href="/music/1329719" class="text-primary font-weight-bold" target="_blank">蓝莲花
//</a>
//    QString downloadDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/mp3/";
////                          ".*<td[^>]*>([^<]*)</td>");

//    QRegularExpression re("class=\"songname[^>]*>[^<]*<a href=\"([^\"]*)\" title=\"([^\"]*)\"[^<]*</a>[^>]*>[^>]*>[^<]*<a href=\"([^\"]*)\" title=\"([^\"]*)\"");

//    QRegularExpressionMatchIterator matches = re.globalMatch(QString::fromUtf8 (data));
//    while (matches.hasNext()) {
//        QRegularExpressionMatch match = matches.next();
//        QString playUrl = match.captured(1);
//        qDebug() << playUrl;
//        QString title = match.captured(2);
//        QString singerUrl = match.captured(3);
//        QString singer = match.captured(4);

//        QString id = playUrl.mid(6, playUrl.size() - 10);
//        QString url = "http://localhost:8080/";
//        QUrl qurl(url);
//        QNetworkAccessManager* manager = new QNetworkAccessManager();
//        QNetworkRequest request(qurl);
//        QNetworkReply* reply = manager->get(request);
//        QObject::connect(reply, &QNetworkReply::finished, [=]() {
//            if (reply->attribute(QNetworkRequest::RedirectionTargetAttribute).isValid()) {
//                QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
//                qDebug() << redirectUrl.toString();
//            }
//            else {
//                qDebug() << reply->readAll();
//            }
//            reply->deleteLater();
//            manager->deleteLater();
//        });

//        qDebug() << playUrl << id << title << singerUrl << singer;
        //        break;
//    }
//    QRegularExpression re("<div class=\"songname [^>]*>[^<]*<a href=\"(.+?)\" title=\"(.+?)\" data-pjax=\"\">[^<]</a>");
//    QRegularExpressionMatchIterator matches = re.globalMatch (QString::fromUtf8 (data));
//    qDebug() << matches.hasNext ();
//    while (matches.hasNext ()) {
//        QRegularExpressionMatch match = matches.next ();
//        qDebug() << match;
//    }
//    QRegularExpression re("<a href=\"/music/([0-9]+)\"[^>]*>([^<]*)</a>[^<]*</td>[^<]*<td[^<]*>([^<]*)</td>");
//    QRegularExpressionMatchIterator matches = re.globalMatch (QString::fromUtf8 (data));
//    qDebug() << matches.hasNext ();
//    while (matches.hasNext ()) {
//        QRegularExpressionMatch match = matches.next ();
//        QString music_id = match.captured(1);
//        QString title = match.captured(2).trimmed ();
//        QString artist = match.captured(3).trimmed ();

////        m_mp3Path = downloadDir + title + "-" + artist + ".mp3";
////        m_lrcPath = downloadDir + title + "-" + artist + ".lrc";

//        qDebug() << music_id << title << artist;
////        qDebug() << m_mp3Path << m_lrcPath;
////        scrapeDetail(music_id);
//        break;
//    }
}

void MusicScraper::parseDetail(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();
//    qDebug() << QString::fromUtf8 (data);
    QString title, artist, mp3Url, lrcUrl;
    if (!parseMusicInfo(data, title, artist, mp3Url, lrcUrl)) {
        std::cerr << "Failed to parse music info for "  << std::endl;//<< url.toStdString()
        return;
    }
    qDebug() << title << artist << mp3Url << lrcUrl;

    QString downloadDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/mp3/";
    m_mp3Path = downloadDir + title + "-" + artist + ".mp3";
    m_lrcPath = downloadDir + title + "-" + artist + ".lrc";

   // QString url = "https://other-web-nf01-sycdn.kuwo.cn/dd4ae13a976cc8c2f49fa1f285d94a03/64441deb/resource/n2/24/48/2849915713.mp3";
    QString mp3Name = QString("%1-%2.mp3").arg (title).arg (artist);
    QString lrcName = QString("%1-%2.lrc").arg (title).arg (artist);
    qDebug() << mp3Name << lrcName;

    downloadMusic (mp3Url,mp3Name);
    downloadMusic (lrcUrl,lrcName);
    qDebug() << "Sending signal from" << this << "with mp3Path:" << m_mp3Path << "and lrcPath:" << m_lrcPath;

    emit sendPath (m_mp3Path,m_lrcPath);
    qDebug() << "下载完成";
}

bool MusicScraper::parseMusicInfo(const QByteArray &data, QString &title, QString &artist, QString &mp3Url, QString &lrcUrl)
{
    QRegularExpression titleRe("<title>(.+?)-(.+?)-");
    QRegularExpressionMatch titleMatch = titleRe.match (QString::fromUtf8 (data));
    if (!titleMatch.hasMatch ()) {
        return false;
    }
    title = titleMatch.captured (2);
    artist = titleMatch.captured (1);
    qDebug() << title << artist;

//    QString rex = "const url = '(.*?)'";
    QRegularExpression mp3Re("const url = '(.*?)'");
    QRegularExpressionMatch mp3Match = mp3Re.match(QString::fromUtf8(data));

    if (!mp3Match.hasMatch()) {
        return false;
    }

    mp3Url = mp3Match.captured(1);
    QRegularExpression lrcRe("<a id=\"btn-download-lrc\" href=\"(.*?)\"");
    QRegularExpressionMatch lrcMatch = lrcRe.match(QString::fromUtf8(data));

    if (!lrcMatch.hasMatch()) {
        return false;
    }
    lrcUrl = "https://www.gequbao.com" + lrcMatch.captured(1);

    return true;
}
