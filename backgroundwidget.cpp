#include "backgroundwidget.h"

#include <QLinearGradient>
#include <QPainter>
#include <QResizeEvent>

BackgroundWidget::BackgroundWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, false);
    setAutoFillBackground(false);
}

void BackgroundWidget::setBackgroundOpacity(qreal opacity)
{
    m_opacity = qBound(0.0, opacity, 1.0);
    update();
}

void BackgroundWidget::setBackgroundImage(const QString &resourcePath)
{
    m_source = QPixmap(resourcePath);
    updateScaledPixmap();
    update();
}

void BackgroundWidget::updateScaledPixmap()
{
    if (m_source.isNull() || size().isEmpty()) {
        m_scaled = QPixmap();
        return;
    }

    m_scaled = m_source.scaled(size() * devicePixelRatioF(),
                               Qt::KeepAspectRatioByExpanding,
                               Qt::SmoothTransformation);
    m_scaled.setDevicePixelRatio(devicePixelRatioF());
}

void BackgroundWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillRect(rect(), m_baseColor);

    if (!m_scaled.isNull()) {
        const qreal dpr = devicePixelRatioF();
        const QSize logical = m_scaled.size() / dpr;
        const int x = (width() - logical.width()) / 2;
        const int y = (height() - logical.height()) / 2;
        painter.setOpacity(m_opacity);
        painter.drawPixmap(x, y, m_scaled);
        painter.setOpacity(1.0);

        QLinearGradient fade(0, height() * 0.08, 0, height() * 0.58);
        fade.setColorAt(0.0, QColor(m_baseColor.red(), m_baseColor.green(), m_baseColor.blue(), 40));
        fade.setColorAt(1.0, m_baseColor);
        painter.fillRect(rect(), fade);
    }
}

void BackgroundWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateScaledPixmap();
}
