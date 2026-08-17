#ifndef CUSTOMDIALOG_H
#define CUSTOMDIALOG_H

#include <QDialog>

class CustomDialog : public QDialog {
    Q_OBJECT

public:
    explicit CustomDialog(const QString &message, QWidget *parent = nullptr);
    static bool showCustomDialog(const QString &message, QWidget *parent = nullptr);

private slots:
    void onContinueClicked();
    void onCancelClicked();

private:
    void setupUI(const QString &message);
};

#endif // CUSTOMDIALOG_H
