#ifndef DESKTOPLYRICS_H
#define DESKTOPLYRICS_H

#include <QLabel>
#include <QString>
#include <QMouseEvent>
#include <QPushButton>
#include <QPaintEvent>
#include <QPainter>
#include <QEvent>
class DesktopLyrics : public QLabel
{
    Q_OBJECT
public:
    explicit DesktopLyrics(QWidget *parent = nullptr);
    ~DesktopLyrics();

    void setText(const QString &text);
public slots:
    void recvText(const QString &text);
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;

    void leaveEvent(QEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

private:
    QPoint m_dragPosition;
    QPushButton *m_closeButton;

    void setupUI();
    void createButtons();

};

#endif // DESKTOPLYRICS_H
