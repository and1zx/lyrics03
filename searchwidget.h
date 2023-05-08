#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>
#include <QList>
#include <QListWidget>
#include <QToolButton>
#include <QListWidgetItem>

#include "musiczz123.h"
#include "fileutils.h"

class SearchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SearchWidget(QWidget *parent = nullptr);
    bool eventFilter(QObject* obj, QEvent* event) override;

public:
    MusicZz123 *m_musicZz123;

private slots:
    void recvList(QList<Item> items);
    void recvSaved(const QString &mp3Path,const QString &lrcPath);

signals:
    void sendPlay(const QList<QString> list);
private:
    QListWidget *m_listWidget;
    QLineEdit *m_searchLine;
    QToolButton *m_searchButton;
    QListWidgetItem *m_currentItem;
    void handleSearch();
    void createUI();
    void connectSignals();
    void loadTestData();
    void onListItemClicked(QListWidgetItem *item,QObject *sender);
};

#endif // SEARCHWIDGET_H
