#include "desktoplyrics.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "lyricparser.h"
#include <QObject>
DesktopLyrics::DesktopLyrics(QWidget *parent) : QLabel(parent) {
//    setStyleSheet ("QLable{background-color:red;color:white:font-size:30px;}");
    setAlignment (Qt::AlignCenter);
    setWordWrap (true);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
//    setWindowOpacity (0.8);
    setFixedSize (1002,63);

    setupUI ();
    createButtons();
}

DesktopLyrics::~DesktopLyrics()
{

}

void DesktopLyrics::setText(const QString &text)
{
    qDebug() << text;
    QLabel::setText (text);
}

void DesktopLyrics::recvText(const QString &text)
{
    setText(text);
}

void DesktopLyrics::mousePressEvent(QMouseEvent *event)
{
    if(event->button () == Qt::LeftButton) {
        m_dragPosition = event->globalPos () - pos ();
        event->accept ();
    }
}

void DesktopLyrics::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons () & Qt::LeftButton) {
        move(event->globalPos () - m_dragPosition);
        event->accept ();
    }
}

void DesktopLyrics::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button () == Qt::LeftButton) {
        m_dragPosition = QPoint();
        event->accept ();
    }
}
void DesktopLyrics::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    m_closeButton->show ();
    m_closeButton->setStyleSheet("border-image: url(:/asserts/close_hover.png);");
}

void DesktopLyrics::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_closeButton->setStyleSheet("border-image: url(:/asserts/close.png);");
}

void DesktopLyrics::setupUI()
{
    setObjectName ("desktopLyrics");
    setStyleSheet("#desktopLyrics{background-color: transparent;}");

//    QFont font("Microsoft YaHei", 23, QFont::Normal);
//    font.setLetterSpacing(QFont::PercentageSpacing, 110);
//    font.setCapitalization(QFont::AllUppercase);
//    font.setStyleStrategy(QFont::PreferAntialias);



    // 设置歌词颜色
//    QPalette palette;
//    palette.setColor(QPalette::WindowText, QColor(255, 255, 255));

//    // 设置样式
//    QString style = "QLabel{background-color: #4F4F4F;"
//                    "border: none;"
//                    "text-align: center;"
//                    "border-radius: 18px;}";

//    setStyleSheet("QLabel {"
//                  "background-color: rgba(0, 0, 0, 100);"
//                  "border-radius: 10px;"
//                  "}");
//    setFont (font);
//    setPalette (palette);
//    setStyleSheet (style);
}

void DesktopLyrics::createButtons()
{
    m_closeButton = new QPushButton(this);
    m_closeButton->setGeometry(10, 10, 16, 16);
//    m_closeButton->setIcon(QIcon(":/asserts/close.png"));
//    m_closeButton->setText("Custom Button");

    m_closeButton->setObjectName ("closeButton");
//    m_closeButton->setStyleSheet("background-color: transparent; font-size: 16px; color: white;");

    m_closeButton->setStyleSheet("#closeButton {"
                                 "background-color: transparent;"
                                 "border-image: url(:/asserts/close.png);"
                                 "}"
                                 "#closeButton:hover {"
                                 "border-image: url(:/asserts/close_hover.png);"
                                 "}"
                                 "#closeButton:pressed {"
                                 "border-image: url(:/asserts/close_hover.png);"
                                 "}");
    m_closeButton->hide ();
    connect(m_closeButton, &QPushButton::clicked, this, &DesktopLyrics::close);


//    m_closeButton = new QPushButton(this);
//    m_closeButton->setFixedSize (12,12);
//    m_closeButton->setContentsMargins (0,0,0,0);
//    m_closeButton->move (0,0);
//    QHBoxLayout *topLayout = new QHBoxLayout();
//    topLayout->setContentsMargins (0,0,0,0);
//    topLayout->setSpacing (0);

//    QWidget *widget = new QWidget();

//    widget->setGeometry (0,0,100,100);

//    topLayout->addWidget (widget);
//    topLayout->setGeometry (0,0,100,100);
//    topLayout->addWidget (m_closeButton);
//    topLayout->setAlignment (Qt::AlignLeft|Qt::AlignTop);
//    topLayout->addStretch();

//    QVBoxLayout *mainLayout = new QVBoxLayout();
//    mainLayout->addLayout (topLayout);
//    setLayout (topLayout);
//    // 设置按钮的样式
//    m_closeButton->setStyleSheet("QPushButton {"
//                                 "background-color: transparent;"
//                                 "border-image: url(:/asserts/close.png);"
//                                 "}"
//                                 "QPushButton:hover {"
//                                 "border-image: url(:/asserts/close_hover.png);"
//                                 "}"
//                                 "QPushButton:pressed {"
//                                 "border-image: url(:/asserts/close_pressed.png);"
//                                 "}");
    //    m_closeButton->move (0, 0);
}

void DesktopLyrics::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    // 绘制半透明背景和圆角
    painter.setBrush(QColor(105, 105, 105, 150));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 5, 5);
    painter.setOpacity (0.8);
    painter.setPen(Qt::white);
//    QFont font("Arial", 30, QFont::Bold);

    QFont font("Microsoft YaHei", 23, QFont::Normal);
    font.setLetterSpacing(QFont::PercentageSpacing, 110);
    font.setCapitalization(QFont::AllUppercase);
    font.setStyleStrategy(QFont::PreferAntialias);

    painter.setFont(font);
    painter.drawText(rect(), Qt::AlignCenter, text());
////    painter.setRenderHint(QPainter::Antialiasing);
////    painter.setPen(Qt::NoPen);

//    QColor color(0, 0, 0, 200);
//    painter.setBrush(color);
//    painter.drawRoundedRect(rect(), 10, 10);


}
