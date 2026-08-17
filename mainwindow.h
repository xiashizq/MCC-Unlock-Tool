#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QString>

class QLabel;
class QToolButton;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void changeEvent(QEvent *event) override;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();

private:
    void applyStyle();
    void setupTableView();
    void setupStatusBar();
    void setupLineEdit();
    void setupLanguageMenu();
    void retranslateUiTexts();

    QString normalizePath(const QString &path) const;
    QString unlockDbPath(const QString &gameDir) const;
    QString backupDbPath(const QString &gameDir) const;
    QString backupDirPath(const QString &gameDir) const;

    bool checkUnlockFile(const QString &gameDir, bool backup = false) const;
    bool ensureBackupDir(const QString &gameDir);
    bool removeIfExists(const QString &filePath);
    bool backupFile(const QString &src, const QString &dstDir);
    bool restoreFile(const QString &gameDir);
    void editXmlFile(const QString &fullFilePath);
    void showXmlFile(const QString &fullFilePath);
    void refreshTableIfValid(const QString &gameDir);

    void saveDirSettings(const QString &dirPath);
    QString loadDirSettings() const;

    QString detectGameFolder();

    void showInfo(const QString &text);
    void showWarn(const QString &text);

    Ui::MainWindow *ui;
    QStandardItemModel *model = nullptr;
    QLabel *m_statusCredit = nullptr;
    QToolButton *m_btnGithub = nullptr;
    QToolButton *m_btnBilibili = nullptr;
    bool m_detectedFromSteam = false;
};

#endif // MAINWINDOW_H
