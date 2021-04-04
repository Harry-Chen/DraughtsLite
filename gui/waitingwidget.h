#ifndef WAITINGWIDGET_H
#define WAITINGWIDGET_H

#include <QWidget>
#include <QLabel>

class WaitingWidget : public QWidget {
    Q_OBJECT

public:
    explicit WaitingWidget(QWidget* parent);

    void enableWidget(const QString &text);
    void disableWidget();

protected:
    void resizeEvent(QResizeEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;

private:
    QLabel *m_textLabel;
    QLabel *m_backgroundLabel;
    QPixmap m_pixmap;
    QString m_lastMessage;
};

#endif // WAITINGWIDGET_H
