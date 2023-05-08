#include <QObject>
#include <QApplication>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QStandardPaths>





#include "lyricparser.h"
#include "desktoplyrics.h"
#include <QFile>

#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>

#include <iostream>

#include <searchwidget.h>
#include "fileutils.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    a.setApplicationName ("音乐播放器");
//    a.setApplicationDisplayName ("音乐播放器");
    SearchWidget searchWidget;
    searchWidget.show ();

    DesktopLyrics desktopLyrics;
    LyricParser lyricParser;

    QMediaPlayer player;
    QAudioOutput *audioOutput = new QAudioOutput();

    desktopLyrics.hide ();
    player.setAudioOutput (audioOutput);
//    player->setBackend(QMediaPlayer::AVFoundation);
    QObject::connect(&player, &QMediaPlayer::positionChanged, [&]() {
        qint64 position = player.position ();
        lyricParser.setCurrentTime (position);
    });
    QObject::connect(&lyricParser, &LyricParser::sendText,
                     &desktopLyrics, &DesktopLyrics::recvText);
    QObject::connect(&searchWidget, &SearchWidget::sendPlay, [&](QList<QString> list) {

        if (desktopLyrics.isHidden ()) {
            desktopLyrics.show ();
        }
        QString lrcPath = FileUtils::getFilePath (list.at (0));
        QString mp3Path = FileUtils::getFilePath (list.at (1));
        QString playState = list.at(2);
        qDebug() << "connect sendPlay" << "mp3Path:"<<mp3Path<< lrcPath;
        qDebug() << "playState" << playState;
        if (playState == "pause") {
            player.stop ();
            return;
        }
        if (!lyricParser.loadFile (lrcPath)) {
            return;
        }
//        //        player.setSource (QUrl("qrc:/asserts/胭脂扣-梅艳芳.mp3"));
        QFile *file = new QFile(mp3Path);
        if (!file->open (QIODevice::ReadOnly)) {
            return;
        }
        player.setSourceDevice (file);

        player.play();
    });


//    QString lrcPath = searchWidget.m_musicZz123->getFilePath ("当我想你的时候-汪峰.lrc");//":/asserts/胭脂扣-梅艳芳.lrc"
//    QString mp3Path = searchWidget.m_musicZz123->getFilePath ("当我想你的时候-汪峰.mp3");
//    qDebug() << lrcPath << mp3Path;
//    if (!lyricParser.loadFile (lrcPath)) {
//        return 0;
//    }
//    //        player.setSource (QUrl("qrc:/asserts/胭脂扣-梅艳芳.mp3"));
//    QFile *file = new QFile(mp3Path);
//    if (!file->open (QIODevice::ReadOnly)) {
//        return 0;
//    }
//    player.setSourceDevice (file);
//    player.play();
//    desktopLyrics.show ();
    return a.exec ();
}
