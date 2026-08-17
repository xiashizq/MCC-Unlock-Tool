#ifndef CUSTOMDIALOG_H
#define CUSTOMDIALOG_H

#include <QDialog>
#include <QPoint>

class QWidget;

class CustomDialog : public QDialog {
    Q_OBJECT

public:
    enum class Kind {
        Confirm,
        Information,
        Warning
    };

    explicit CustomDialog(Kind kind, const QString &message, QWidget *parent = nullptr);

    static bool showCustomDialog(const QString &message, QWidget *parent = nullptr);
    static void showInformation(const QString &message, QWidget *parent = nullptr);
    static void showWarning(const QString &message, QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onContinueClicked();
    void onCancelClicked();

private:
    void setupUI(Kind kind, const QString &message);

    QWidget *m_titleBar = nullptr;
    bool m_dragging = false;
    QPoint m_dragOffset;
};

#endif // CUSTOMDIALOG_H
