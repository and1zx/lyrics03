#ifndef LYRICPARSER_H
#define LYRICPARSER_H

#include <QObject>
#include <QStringView>
#include <QMap>

struct Lyrics
{
    int time;
    QString text;
    bool operator<(const Lyrics &other) const
    {
        return time < other.time;
    }
};
class LyricParser : public QObject
{
    Q_OBJECT
public:
    explicit LyricParser(QObject *parent = nullptr);
    bool loadFile(const QString &filename);
    int lineCount() const;
    const Lyrics& lineAt(int index) const;
    void setCurrentTime(int time);
signals:
    void sendText(const QString &text);

private:
    void parseLine(const QString &line);
    QList<Lyrics> lyricsList;
    int currentIndex;
    QString currentLyrics;
};

#endif // LYRICPARSER_H
