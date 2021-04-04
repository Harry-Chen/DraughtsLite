#include <QPainter>
#include <QtEvents>
#include "waitingwidget.h"

WaitingWidget::WaitingWidget(QWidget* parent) : QWidget(parent)
{
    m_backgroundLabel = new QLabel(this);
    m_textLabel = new QLabel(this);

    m_textLabel->setAlignment(Qt::AlignCenter);
    m_textLabel->setStyleSheet("font-size: 15px; font-weight: bold; color : white;");

    m_backgroundLabel->hide();
    m_textLabel->hide();

    this->hide();
}


void WaitingWidget::enableWidget(const QString &text)
{
    m_lastMessage = text;
    m_backgroundLabel->show();
    m_textLabel->setText(m_lastMessage);
    m_textLabel->show();
    this->show();
}

void WaitingWidget::disableWidget()
{
    m_backgroundLabel->hide();
    m_textLabel->hide();
    this->hide();
}

void WaitingWidget::resizeEvent(QResizeEvent *e)
{
    m_pixmap = QPixmap(e->size());
    m_pixmap.fill(Qt::transparent);

    QPainter p(&m_pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(Qt::black, 1));

    QPainterPath path;
    path.addRoundedRect(QRectF(0.5, 0.5, (qreal)this->width() - 0.5, (qreal)this->height() - 0.5), 16, 16);

    p.fillPath(path, QColor(0, 0, 0, 150));
    p.drawPath(path);


    m_textLabel->resize(this->size());
    m_backgroundLabel->resize(this->size());
    m_backgroundLabel->setPixmap(m_pixmap);
}

void WaitingWidget::mousePressEvent(QMouseEvent *e)
{
    e->accept();
}
