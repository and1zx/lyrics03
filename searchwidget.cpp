#include "searchwidget.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QHBoxLayout>
#include <QToolButton>
#include <QListWidgetItem>
#include <QLabel>
#include <QEvent>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QScrollBar>

SearchWidget::SearchWidget(QWidget *parent)
    : QWidget{parent},
    m_musicZz123(new MusicZz123()),
    m_listWidget(new QListWidget()),
    m_searchLine(new QLineEdit()),
    m_searchButton(new QToolButton()),
    m_currentItem(nullptr)
{
    setFixedSize (300,400);
    createUI ();
    connectSignals ();
    loadTestData ();
}

bool SearchWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Enter) {
        QLabel* label = qobject_cast<QLabel*>(obj);
        if (label) {
            QString text = label->text();
            if (label->fontMetrics().horizontalAdvance(text) > label->width()) {
//                auto animation = new QPropertyAnimation(label, "pos");
//                animation->setDuration(3000);
//                animation->setStartValue(QPointF(0, 0));
//                QPointF endPos(-50.0, 0);
//                qDebug() << label->width() << label->fontMetrics().horizontalAdvance(text);

//                animation->setEndValue(endPos);
//                animation->setEasingCurve(QEasingCurve::Linear);
//                animation->start(QAbstractAnimation::DeleteWhenStopped);
//                label->setProperty("animation", QVariant::fromValue(animation));
            }
        }
    } else if (event->type() == QEvent::Leave) {
        if (auto label = qobject_cast<QLabel*>(obj)) {
            if (auto animation = label->property("animation").value<QPropertyAnimation*>()) {
                animation->stop();
                label->setGeometry(0, 0, label->width(), label->height());
                label->setProperty("animation", QVariant());
            }
        }
    }
//    else if (event->type() == QEvent::MouseMove) {
//        if (auto label = qobject_cast<QLabel*>(obj)) {
//            QPoint pos = static_cast<QMouseEvent*>(event)->pos();
//            if (label->fontMetrics().horizontalAdvance(label->text()) > label->width() &&
//                (pos.x() < label->fontMetrics().horizontalAdvance(label->text()) - label->width())) {
//                if (auto animation = label->property("animation").value<QPropertyAnimation*>()) {
//                    animation->pause();
//                }
//                label->setIndent(pos.x());
//            } else {
//                if (auto animation = label->property("animation").value<QPropertyAnimation*>()) {
//                    animation->resume();
//                }
//                label->setIndent(0);
//            }
//        }
//    }

    return QWidget::eventFilter(obj, event);
}


void SearchWidget::recvList(QList<Item> items)
{
    qDebug() << "recvList";
    m_listWidget->blockSignals (true);
    // 将每个结构体Item的信息添加到QListWidget中
    for (const auto& item : items) {
        QWidget* widget = new QWidget;
        QHBoxLayout* h_layout = new QHBoxLayout(widget);
        QVBoxLayout* v_layout = new QVBoxLayout();
        QLabel* title_label = new QLabel(item.title,widget);
        QFontMetrics font_metrics(title_label->font());
        QString elided_title = font_metrics.elidedText(item.title, Qt::ElideRight, title_label->width());

        // 设置ToolTip
//        title_label->setToolTip(item.title);
        title_label->setStyleSheet ("font-size:14px;");
//        title_label->installEventFilter (this);

        QLabel* artist_label = new QLabel(item.artist,widget);
        artist_label->setStyleSheet ("font-size:12px;color:#A3A7AD;");

        QToolButton* playButton = new QToolButton(widget);
        playButton->setObjectName ("playButton");
        playButton->setIcon (QIcon(":/asserts/play.png"));

        QToolButton* download_button = new QToolButton(widget);
        download_button->setObjectName ("downloadButton");
        download_button->setIcon(QIcon(":/asserts/download.png"));

        v_layout->addWidget (title_label);
        v_layout->addWidget (artist_label);
        v_layout->setSpacing (0);
        v_layout->setContentsMargins (0,0,0,0);

        h_layout->addLayout (v_layout);
        h_layout->addStretch ();
        h_layout->addWidget (playButton);
        h_layout->addWidget(download_button);

        h_layout->setSpacing (10);
        h_layout->setContentsMargins (0,0,0,0);

        QListWidgetItem* listItem = new QListWidgetItem(m_listWidget);
        listItem->setSizeHint (QSize(0,44));
        QVariant data;
        data.setValue (item);
        listItem->setData(Qt::UserRole, data);
        widget->setCursor(Qt::PointingHandCursor);

        playButton->setStyleSheet("background-color: transparent;");
        download_button->setStyleSheet("background-color: transparent;");
        playButton->setProperty ("state","play");
        m_listWidget->setItemWidget(listItem,widget);

        connect(playButton, &QToolButton::clicked,this, [=]() {
            onListItemClicked(listItem,sender());
        });
    }
    m_listWidget->blockSignals (false);
}
void SearchWidget::recvSaved(const QString &songName,const QString &singerName)
{
    qDebug() << "recvSaved:" << songName << singerName;
    QString lrcName = songName +"-"+ singerName + ".lrc";
    QString mp3Name = songName +"-"+ singerName + ".mp3";
    QList<QString> list;
    list.append (lrcName);
    list.append (mp3Name);
    list.append ("play");
    emit sendPlay (list);
}

void SearchWidget::handleSearch()
{
    qDebug() << "handleSearch";
    QString keyword = m_searchLine->text ().trimmed ();
    if (keyword.isEmpty ()) {
        qDebug() << "handleSearch :keyword empty";
        return;
    }
    m_listWidget->clear ();
    m_musicZz123->search(keyword);
}

void SearchWidget::createUI()
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins (5,5,5,5);
    setStyleSheet ("background-color: #ffffff;");
    QWidget *searchContainer = new QWidget();
    searchContainer->setStyleSheet("QWidget {"
                                   "background-color: #f5f5f5;"
                                   "border-radius: 20px;"
                                   "height:25px;"
                                   "margin: 0;"
                                   "padding: 0;"
                                   "}");
    QHBoxLayout *searchLayout = new QHBoxLayout(searchContainer);
    searchLayout->setSpacing (0);
    searchLayout->setContentsMargins (0,2,0,2);

    m_searchButton = new QToolButton();
    m_searchButton->setIcon(QIcon(":/asserts/search.png"));  // 设置搜索图标
    m_searchButton->setStyleSheet("QToolButton {"
                                "background-color: transparent;"
                                "width:30px;"
                                "}");
    m_searchButton->setCursor(Qt::PointingHandCursor);

    m_listWidget->setStyleSheet("QListWidget { background-color: white; border: none; }"
                                "QListWidget::item { border-bottom: 1px solid rgba(0, 0, 0, 0.1); }");
    m_listWidget->verticalScrollBar()->setStyleSheet("QScrollBar::vertical { border: none; background: #F1F1F1; width: 4px; margin: 0px 0px 0px 0px; } QScrollBar::handle:vertical { background: #D8D8D8; min-height: 20px; } QScrollBar::add-line:vertical { border: none; background: none; } QScrollBar::sub-line:vertical { border: none; background: none; } QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { border: none; background: none; color: none; height: 0px; } QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }");

    m_listWidget->setStyleSheet("QListWidget {"
                                "background-color: #FFFFFF;"
                                "border: none;"
                                "border-radius: 5px;"
                                "padding: 5px;"
                                "}"
                                "QListWidget::item {"
                                "border-bottom: 1px solid rgba(0, 0, 0, 0.1);"
                                "}"
                                "QListWidget::item:hover {"
                                "background-color: rgba(0, 0, 0, 0.05);"
                                "}"
                                "QListWidget::item:selected {"
                                "background-color: rgba(0, 0, 0, 0.1);"
                                "}"
                                "QToolButton {"
                                "background-color: transparent;"
                                "border: none;"
                                "}"
                                "QToolButton:hover {"
                                "background-color: rgba(0, 0, 0, 0.05);"
                                "}");
    m_searchLine->setStyleSheet("QLineEdit {"
                                "border: 0px solid gray;"
                                "border-radius: 5px;"
                                "padding: 0 0px;"
                                "margin:0;padding:0"
                                "}");

    searchLayout->addWidget (m_searchLine);
    searchLayout->addWidget (m_searchButton);

    layout->addWidget (searchContainer);
    layout->addWidget (m_listWidget);
    setLayout (layout);
}

void SearchWidget::connectSignals()
{
    connect (m_searchButton,&QToolButton::clicked,this,&SearchWidget::handleSearch);
    connect(m_musicZz123, &MusicZz123::sendList, this, &SearchWidget::recvList);
    connect(m_musicZz123, &MusicZz123::sendSaved, this, &SearchWidget::recvSaved);
    connect(m_searchLine, &QLineEdit::returnPressed, this, &SearchWidget::handleSearch);
}

void SearchWidget::loadTestData()
{
    // 创建结构体Item的QList作为数据源
    QList<Item> items = {
        {"想念真好想念真好想念真好1想念真好想念真好想念真好想2念真好想念真好想念真好想3念真好", "汪峰", "id1", "file1"},
        {"春天里", "汪峰", "aksmsd", "file2"},
        {"当我想你的时候", "汪峰", "akvkqa", "file2"},
        {"怒放的生命 《北京青年》电视剧插曲", "汪峰", "dvakv", "file2"},
        {"存在 电视剧《北京青年》片尾曲", "汪峰", "vavzzq", "file2"},
        {"飞的更高", "汪峰", "dudzx", "file2"},
        {"北京北京", "汪峰", "kzsmm", "file2"},
        {"像梦一样自由", "汪峰", "vxqddk", "file2"},
        {"勇敢的心", "汪峰", "kuavm", "file2"},
        {"一起摇摆", "汪峰", "uzmvvx", "file2"},
        {"在雨中", "汪峰", "ssxmq", "file2"},
        {"生来彷徨", "汪峰", "uzdauz", "file2"},
        {"我爱你中国", "汪峰", "akdqzz", "file2"},
        {"飞得更高", "汪峰", "zkkxx", "file2"},
        {"美丽世界的孤儿", "汪峰", "zkkxm", "file3"}
    };
    recvList (items);
}

void SearchWidget::onListItemClicked(QListWidgetItem *item,QObject *sender)
{
        QVariant data = item->data(Qt::UserRole);
        Item it = data.value<Item>();
        qDebug() << "onListItemClicked:" << it.title << it.artist << it.id;
        if (m_currentItem != nullptr && m_currentItem != item) {
            QWidget *widget = qobject_cast<QWidget*>(m_listWidget->itemWidget (m_currentItem));
            QToolButton* playButton = widget->findChild<QToolButton*>("playButton");
            playButton->setProperty ("state","play");
            playButton->setIcon(QIcon(":/asserts/play.png"));
        }

        m_currentItem = item;
        QWidget *widget = qobject_cast<QWidget*>(m_listWidget->itemWidget (item));
        QToolButton* playButton = widget->findChild<QToolButton*>("playButton");
        if (sender == playButton) {
            QList<QString> list;
            QString lrcName = it.title +"-"+ it.artist + ".lrc";
            QString mp3Name = it.title +"-"+ it.artist + ".mp3";
            list.append (lrcName);
            list.append (mp3Name);
            qDebug() << "lrcName:" << FileUtils::exists (lrcName);
            qDebug() << "mp3Name:" << FileUtils::exists (mp3Name);
            QString state = playButton->property("state").toString();
            if (!FileUtils::exists (mp3Name)) {
                qDebug() << "mp3Name 不存在" << mp3Name;
                playButton->setProperty("state", "play");
                playButton->setIcon(QIcon(":/asserts/play.png"));
                list.append ("pause");
                m_musicZz123->sendSongInfo (it.id);
                return;
            }


            if (state == "play") {
                playButton->setProperty("state", "pause");
                playButton->setIcon(QIcon(":/asserts/pause.png"));
                list.append ("play");
                // TODO: 播放歌曲
            } else {
                playButton->setProperty("state", "play");
                playButton->setIcon(QIcon(":/asserts/play.png"));
                list.append ("pause");
                // TODO: 暂停歌曲
            }
            emit sendPlay (list);
        }
//        else if (button == downloadButton) {
//            // TODO: 下载歌曲
//        }
}
