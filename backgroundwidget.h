#ifndef BACKGROUNDWIDGET_H
#define BACKGROUNDWIDGET_H

#include <QPixmap>
#include <QWidget>

class BackgroundWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal backgroundOpacity READ backgroundOpacity WRITE setBackgroundOpacity)

public:
    explicit BackgroundWidget(QWidget *parent = nullptr);

    qreal backgroundOpacity() const { return m_opacity; }
    void setBackgroundOpacity(qreal opacity);

    void setBackgroundImage(const QString &resourcePath);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateScaledPixmap();

    QPixmap m_source;
    QPixmap m_scaled;
    qreal m_opacity = 0.22;
    QColor m_baseColor{0x0b, 0x0d, 0x10};
};

#endif // BACKGROUNDWIDGET_H
