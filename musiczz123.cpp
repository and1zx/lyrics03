#include "musiczz123.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QDir>
#include <QEventLoop>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>
#include "fileutils.h"
MusicZz123::MusicZz123(QObject *parent)
    : QObject{parent},
    m_manager(new QNetworkAccessManager(this))
{
    qDebug() << "MusicZz123";
//    connect (m_manager,&QNetworkAccessManager::finished ,this,&MusicZz123::handleNetworkReply);
}

void MusicZz123::sendSongInfo(const QString &id)
{
    QUrlQuery query;
    query.addQueryItem ("act","songinfo");
    query.addQueryItem ("id",id);
    query.addQueryItem ("lang","");
    QNetworkRequest request(QUrl("https://zz123.com/ajax/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *reply = m_manager->post(request, query.toString(QUrl::FullyEncoded).toUtf8());
    QObject::connect(reply, &QNetworkReply::finished, this,[=]() {
        onSongInfoReceived(reply);
    });
}

void MusicZz123::onSongInfoReceived(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "error:"<<reply->error ();
        reply->deleteLater();
        return;
    }
    const QByteArray data = reply->readAll ();
    const QJsonDocument jsonDoc = QJsonDocument::fromJson (data);
    const QJsonObject &songInfo = jsonDoc.object ().value ("data").toObject ();

    m_songName = songInfo.value ("mname").toString ();
    m_singerName = songInfo.value ("sname").toString ();
    QString lrcData = songInfo.value ("lrc").toString ();
    QString mp3Url = songInfo.value ("mp3").toString ();

    qDebug() << mp3Url << m_songName << m_singerName;
    const QString lrcName = QString("%1-%2.lrc").arg(m_songName, m_singerName);
    const QString mp3Name = QString("%1-%2.mp3").arg(m_songName, m_singerName);
    save (lrcName,lrcData.toUtf8 ());

    QUrl songUrl;
    if (mp3Url.startsWith ("http")) {
        songUrl = QUrl(mp3Url);
        qDebug() << "mp3Url" << songUrl << " start download";
    }else {
        QString url = QString("https://zz123.com%1").arg (mp3Url);
        songUrl = QUrl(url);
        qDebug() <<"songUrl"<< songUrl.toString ();
    }
    sendSongLocation (songUrl);
    reply->deleteLater ();
}

void MusicZz123::sendSongLocation(const QUrl &url)
{
    qDebug() << "sendSongLocation" << url.toString ();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setHeader (QNetworkRequest::UserAgentHeader,"Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 Chrome/77.0.3865.90 Safari/537.36");
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    QNetworkReply* reply = m_manager->get(request);
    QObject::connect (reply,&QNetworkReply::finished,this,[=](){
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString location = reply->rawHeader("location");
        qDebug() << "location:" << location << " statusCode:" << statusCode;
        if (statusCode == 301 || statusCode == 302) {
            qDebug() << "onSongLocationReceived" << location;
            onSongLocationReceived(reply);
        }else{
            const QString mp3Name = QString("%1-%2.mp3").arg(m_songName, m_singerName);
            qDebug() << "save mp3" << mp3Name;
            const QByteArray data = reply->readAll();
            save (mp3Name,data);
            reply->deleteLater();
        }
    });
    QObject::connect(reply, &QNetworkReply::redirected, [=](const QUrl &url){
        // 处理重定向
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << "mp3Url 重定向:" << url.toString () << "statusCode:" << statusCode;
        sendSongLocation(url);
        reply->deleteLater();
    });
}

void MusicZz123::onSongLocationReceived(QNetworkReply *reply)
{
    QUrl redirectUrl = QUrl(reply->rawHeader("location"));
    qDebug() << "onSongLocationReceived:" << redirectUrl.toString ();
    reply->deleteLater();
    sendSongLocation (redirectUrl);
}

void MusicZz123::search(const QString &keyword)
{
    QString url = QString("https://zz123.com/search/?key=%1").arg (keyword);
    qDebug() << "search url:" << url;
    QUrl qurl(url);
    QNetworkRequest request(qurl);
    QNetworkReply *reply = m_manager->get (request);
    QObject::connect (reply,&QNetworkReply::finished,this,[=](){
        QString url = reply->url ().toString ();
        qDebug() << "search connect url:"<<url << "url path:"<< reply->url ().path ();
        if (reply->error () != QNetworkReply::NoError) {
            std::cerr << "Network request failed: " << reply->errorString ().toStdString () << std::endl;
            reply->deleteLater ();
            return;
        }

        parseList (reply);
    });
}

void MusicZz123::parseList(QNetworkReply *reply)
{
    qDebug() << "parseList" << reply->url ();
    QByteArray data = reply->readAll ();
    QRegularExpression re("class=\"songname[^>]*>[^<]*<a href=\"([^\"]*)\" title=\"([^\"]*)\"[^<]*</a>[^>]*>[^>]*>[^<]*<a href=\"([^\"]*)\" title=\"([^\"]*)\"");
    QRegularExpressionMatchIterator matches = re.globalMatch(QString::fromUtf8 (data));
    items.clear ();
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        Item item;
        QString playUrl = match.captured(1);
        QString singerUrl = match.captured(3);
        item.title = match.captured(2);
        item.artist = match.captured(4);
        item.id= playUrl.mid(6, playUrl.size() - 10);
        item.fileName  = item.title + "-" + item.artist + ".mp3";
        items.append (item);
        qDebug() << playUrl << item.id << item.title << singerUrl << item.artist;
    }
    emit sendList (items);
    return;
}



void MusicZz123::save(const QString &fileName,const QByteArray &data)
{
    QString downloadDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/mp3/";
    QString filePath = downloadDir + fileName;
    qDebug() << "data size:" << data.size () << filePath;
    QDir dir(downloadDir);
    if (!dir.exists ()) {
        dir.mkpath (downloadDir);
    }
    QFile file(filePath);

    if (file.exists ()) {
        qDebug() << "file exists " << filePath;
        return;
    }
    if (file.open (QIODevice::WriteOnly)) {
        file.write (data);
        std::cout << "Downloaded music to " << filePath.toStdString() << std::endl;
    }else{
        std::cerr << "Failed to open file " << filePath.toStdString() << " for writing" << std::endl;
    }
    file.close ();
}

