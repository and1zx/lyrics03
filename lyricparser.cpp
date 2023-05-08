#include "lyricparser.h"
#include <QFile>
#include <QDebug>
#include <QRegularExpression>
#include <QTime>
#include <stdio.h>

LyricParser::LyricParser(QObject *parent)
    : QObject{parent},currentIndex(-1)
{

}

bool LyricParser::loadFile(const QString &filename)
{
    lyricsList.clear ();
    QFile file(filename);
    if (!file.open (QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << filename;
        return false;
    }
    QTextStream stream(&file);
    while (!stream.atEnd ()) {
        QString line = stream.readLine ().trimmed ();
        if (line.isEmpty ()) {
            continue;
        }
        if (!line.startsWith ("[")) {
            continue;
        }
        parseLine (line);
    }
    std::sort (lyricsList.begin (),lyricsList.end ());
    return true;
}

int LyricParser::lineCount() const
{
    return lyricsList.size ();
}

const Lyrics &LyricParser::lineAt(int index) const
{
    return lyricsList[index];
}

void LyricParser::setCurrentTime(int time)
{
    if (lyricsList.isEmpty ()) {
        return;
    }
    if (currentIndex == -1) {
        if (time < lyricsList.first ().time) {
            return;
        }
        currentIndex = 0;
    }
    if (currentIndex < lyricsList.count () -1 && time > lyricsList.at (currentIndex+1).time) {
        while (currentIndex < lyricsList.count() - 1 && time >= lyricsList.at(currentIndex + 1).time) {
            currentIndex++;
        }
    }else if(currentIndex > 0 && time < lyricsList.at (currentIndex).time) {
        while (currentIndex > 0 && time < lyricsList.at(currentIndex).time) {
            currentIndex--;
        }
    }
    if (currentIndex >=0 && currentIndex < lyricsList.count ()) {
        QString currentLyricsText = lyricsList.at(currentIndex).text;
        if (currentLyricsText != currentLyrics) {
            currentLyrics = currentLyricsText;
            emit sendText (currentLyricsText);
        }
    }
}



void LyricParser::parseLine(const QString &line)
{
    QStringList parts = line.split ("]");
    QString text = parts.last ().trimmed ();
    if(text.isEmpty ()) {
        return;
    }
    for (int i = 0; i < parts.count () -1; ++i) {
        QString timerStr = parts.at (i).trimmed ().mid (1);
        QRegularExpression *re = new QRegularExpression(("(\\d+):(\\d+(\\.\\d+)?)"));
        QRegularExpressionMatch match =  re->match (timerStr);


        int minutes = match.captured (1).toInt ();
        double seconds = match.captured (2).toDouble ();

        int msecs = (minutes * 60 + seconds) * 1000;
        lyricsList.append ({msecs,text});
    }

//    QString pattern = "(\\[(\\d+):(\\d+(?:\\.\\d+)?)\\])";
//    QRegularExpression re(pattern);
//    QRegularExpressionMatch match;

//    int pos = 0;
//    QVector<int> times;
//    while ((match = re.match (line,pos)).hasMatch ()) {
//        QStringList capturedTexts = match.capturedTexts();
//        pos = match.capturedEnd ();
//        QString timeStr = capturedTexts.at (1).mid (1,8);
//        qDebug() << capturedTexts.at (1);
//        int time = QTime::fromString(timeStr, "mm:ss.zz").msecsSinceStartOfDay();
//        times.append (time);
//    }
//    QString text = line.mid (pos);
//    if (text.isEmpty ()) {
//        return;
//    }
//    for (int i = times.size ()-1; i >= 0; --i) {
//        lyricsList.append ({times.at (i),text});
//    }
}
