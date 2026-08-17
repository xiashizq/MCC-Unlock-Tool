#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Customdialog.h"
#include "translationmanager.h"
#include "steampathfinder.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QButtonGroup>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QDomDocument>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFont>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QSettings>
#include <QStatusBar>
#include <QSvgRenderer>
#include <QTextStream>
#include <QToolButton>
#include <QUrl>

#pragma execution_character_set("utf-8")

namespace {
QString stateToDisplay(const QString &state)
{
    if (state == QLatin1String("eUnlockState_Unlocked")) {
        return QObject::tr("Unlocked");
    }
    if (state == QLatin1String("eUnlockState_LockedVisible")) {
        return QObject::tr("Locked (Visible)");
    }
    if (state == QLatin1String("eUnlockState_LockedHidden")) {
        return QObject::tr("Locked (Hidden)");
    }
    return state;
}

QIcon statusBarIcon(const QString &resourcePath)
{
    QSvgRenderer renderer(resourcePath);
    const qreal dpr = qApp ? qApp->devicePixelRatio() : 1.0;
    QPixmap pix(QSize(36, 36) * dpr);
    pix.setDevicePixelRatio(dpr);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing, true);
    renderer.render(&painter);
    return QIcon(pix);
}

QToolButton *makeStatusLinkButton(QWidget *parent, const QString &iconPath, const QUrl &url)
{
    auto *button = new QToolButton(parent);
    button->setAutoRaise(true);
    button->setCursor(Qt::PointingHandCursor);
    button->setIcon(statusBarIcon(iconPath));
    button->setIconSize(QSize(18, 18));
    button->setFocusPolicy(Qt::NoFocus);
    QObject::connect(button, &QToolButton::clicked, parent, [url]() {
        QDesktopServices::openUrl(url);
    });
    return button;
}
} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->setBackgroundImage(QStringLiteral(":/img/img/bg.jpg"));
    ui->centralwidget->setBackgroundOpacity(0.26);

    applyStyle();
    setupTableView();
    setupStatusBar();
    setupLanguageMenu();
    setupLineEdit();
    retranslateUiTexts();

    setWindowIcon(QIcon(QStringLiteral(":/img/img/mcclauncher.ico")));
    ui->statusBar->setSizeGripEnabled(true);
    if (m_detectedFromSteam) {
        ui->statusBar->showMessage(tr("Detected Halo MCC from Steam"), 5000);
    }

    connect(&TranslationManager::instance(), &TranslationManager::languageChanged,
            this, [this](const QString &) {
        retranslateUiTexts();
        const QString path = normalizePath(ui->lineEdit->text());
        if (!path.isEmpty() && checkUnlockFile(path)) {
            showXmlFile(unlockDbPath(path));
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        retranslateUiTexts();
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::applyStyle()
{
    setStyleSheet(QStringLiteral(
        "QMainWindow {"
        "  background-color: #0b0d10;"
        "}"
        "QFrame#workspace {"
        "  background-color: rgba(20, 24, 30, 210);"
        "  border: 1px solid #2a3038;"
        "  border-radius: 6px;"
        "}"
        "QWidget#toolbar {"
        "  background: transparent;"
        "}"
        "QFrame#hairline {"
        "  background-color: #2a3038;"
        "  border: none;"
        "  max-height: 1px;"
        "}"
        "QFrame#langSwitch {"
        "  background-color: rgba(16, 18, 22, 180);"
        "  border: 1px solid #2a3038;"
        "  border-radius: 6px;"
        "}"
        "QLabel {"
        "  color: #c8ced6;"
        "  background: transparent;"
        "}"
        "QLabel#labelHeader {"
        "  color: #f4f6f8;"
        "}"
        "QLabel#label_3 {"
        "  color: #8a929c;"
        "}"
        "QLineEdit {"
        "  background-color: #101318;"
        "  color: #eef1f4;"
        "  border: 1px solid #323840;"
        "  border-radius: 4px;"
        "  padding: 7px 12px;"
        "  selection-background-color: #3d5360;"
        "}"
        "QLineEdit:focus {"
        "  border: 1px solid #6ba3b5;"
        "}"
        "QPushButton {"
        "  background-color: #1c222a;"
        "  color: #e6eaef;"
        "  border: 1px solid #3a424c;"
        "  border-radius: 4px;"
        "  padding: 6px 14px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #262d36;"
        "  border-color: #4b5562;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #161b21;"
        "}"
        "QPushButton#pushButton_2 {"
        "  background-color: #6ba3b5;"
        "  color: #0b1014;"
        "  border: 1px solid #6ba3b5;"
        "  min-width: 120px;"
        "}"
        "QPushButton#pushButton_2:hover {"
        "  background-color: #7db3c4;"
        "  border-color: #7db3c4;"
        "}"
        "QPushButton#pushButton_2:pressed {"
        "  background-color: #5a8fa0;"
        "}"
        "QPushButton#btnLangEn, QPushButton#btnLangZh {"
        "  background: transparent;"
        "  border: none;"
        "  border-radius: 4px;"
        "  color: #8a929c;"
        "  padding: 4px 12px;"
        "  min-width: 40px;"
        "  min-height: 26px;"
        "}"
        "QPushButton#btnLangEn:checked, QPushButton#btnLangZh:checked {"
        "  background-color: #262d36;"
        "  color: #eef1f4;"
        "}"
        "QPushButton#btnLangEn:hover, QPushButton#btnLangZh:hover {"
        "  color: #eef1f4;"
        "}"
        "QTableView {"
        "  background-color: transparent;"
        "  alternate-background-color: rgba(255, 255, 255, 8);"
        "  color: #d8dee6;"
        "  border: none;"
        "  selection-background-color: rgba(107, 163, 181, 48);"
        "  selection-color: #f4f8fa;"
        "  outline: none;"
        "}"
        "QHeaderView::section {"
        "  background-color: #161a20;"
        "  color: #9aa3ae;"
        "  padding: 10px 12px;"
        "  border: none;"
        "  border-bottom: 1px solid #2a3038;"
        "  border-right: 1px solid #2a3038;"
        "}"
        "QTableView::item {"
        "  padding: 6px 12px;"
        "  border-bottom: 1px solid #1c222a;"
        "}"
        "QScrollBar:vertical {"
        "  background: transparent;"
        "  width: 8px;"
        "  margin: 4px 2px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background: #3a424c;"
        "  border-radius: 4px;"
        "  min-height: 28px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "  background: #536074;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical,"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "  background: none;"
        "  height: 0;"
        "}"
        "QStatusBar {"
        "  background-color: #0b0d10;"
        "  color: #9aa3ae;"
        "  border-top: 1px solid #1c222a;"
        "}"
        "QStatusBar QLabel {"
        "  color: #9aa3ae;"
        "}"
        "QStatusBar QToolButton {"
        "  background: transparent;"
        "  border: none;"
        "  padding: 2px 5px;"
        "  margin: 0 1px;"
        "}"
        "QStatusBar QToolButton:hover {"
        "  background-color: #262d36;"
        "  border-radius: 4px;"
        "}"
        "QStatusBar QToolButton:pressed {"
        "  background-color: #1c222a;"
        "}"
    ));

    QFont titleFont = font();
    titleFont.setPointSize(16);
    titleFont.setHintingPreference(QFont::PreferNoHinting);
    ui->labelHeader->setFont(titleFont);

    QFont hintFont = font();
    hintFont.setPointSize(9);
    hintFont.setHintingPreference(QFont::PreferNoHinting);
    ui->label_3->setFont(hintFont);
}

void MainWindow::setupTableView()
{
    model = new QStandardItemModel(this);
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableView->verticalHeader()->setDefaultSectionSize(32);
    ui->tableView->setSortingEnabled(true);
    model->setHorizontalHeaderLabels({tr("ID"), tr("Type"), tr("Category"), tr("State")});
}

void MainWindow::setupStatusBar()
{
    m_statusCredit = new QLabel(this);

    m_btnBilibili = makeStatusLinkButton(
        this,
        QStringLiteral(":/img/img/bilibili.svg"),
        QUrl(QStringLiteral("https://space.bilibili.com/2450808")));
    m_btnGithub = makeStatusLinkButton(
        this,
        QStringLiteral(":/img/img/github.svg"),
        QUrl(QStringLiteral("https://github.com/xiashizq")));

    ui->statusBar->addPermanentWidget(m_statusCredit);
    ui->statusBar->addPermanentWidget(m_btnBilibili);
    ui->statusBar->addPermanentWidget(m_btnGithub);
}

void MainWindow::setupLanguageMenu()
{
    auto *group = new QButtonGroup(this);
    group->setExclusive(true);
    group->addButton(ui->btnLangEn);
    group->addButton(ui->btnLangZh);

    const QString lang = TranslationManager::instance().currentLanguage();
    ui->btnLangEn->setChecked(lang != QLatin1String("zh_CN"));
    ui->btnLangZh->setChecked(lang == QLatin1String("zh_CN"));

    connect(ui->btnLangEn, &QPushButton::clicked, this, []() {
        TranslationManager::instance().setLanguage(QStringLiteral("en"));
    });
    connect(ui->btnLangZh, &QPushButton::clicked, this, []() {
        TranslationManager::instance().setLanguage(QStringLiteral("zh_CN"));
    });
}

void MainWindow::retranslateUiTexts()
{
    setWindowTitle(tr("Halo Armor / Nameplate Unlock Tool  v1.3"));
    ui->labelHeader->setText(tr("Armor & Nameplate Unlock"));
    ui->label_3->setText(tr("Select the MCC install folder, then backup before unlocking."));
    if (m_statusCredit) {
        m_statusCredit->setText(tr("By Mumian Youji"));
    }
    if (m_btnGithub) {
        m_btnGithub->setToolTip(tr("GitHub: xiashizq"));
    }
    if (m_btnBilibili) {
        m_btnBilibili->setToolTip(tr("Bilibili: UID2450808"));
    }
    ui->statusBar->showMessage(tr("Ready"), 2000);

    const QString lang = TranslationManager::instance().currentLanguage();
    ui->btnLangEn->setChecked(lang != QLatin1String("zh_CN"));
    ui->btnLangZh->setChecked(lang == QLatin1String("zh_CN"));

    if (model && model->columnCount() >= 4) {
        model->setHorizontalHeaderLabels({tr("ID"), tr("Type"), tr("Category"), tr("State")});
    }
}

void MainWindow::setupLineEdit()
{
    QString path = loadDirSettings().trimmed();
    if (!checkUnlockFile(normalizePath(path))) {
        const QString detected = detectGameFolder();
        if (!detected.isEmpty()) {
            path = detected;
            m_detectedFromSteam = true;
            saveDirSettings(path);
        }
    }

    if (!path.isEmpty()) {
        ui->lineEdit->setText(QDir::toNativeSeparators(path));
        refreshTableIfValid(normalizePath(path));
    }

    connect(ui->lineEdit, &QLineEdit::editingFinished, this, [this]() {
        const QString text = ui->lineEdit->text().trimmed();
        saveDirSettings(text);
        refreshTableIfValid(normalizePath(text));
    });
}

QString MainWindow::detectGameFolder()
{
    return SteamPathFinder::findHaloMcc();
}

void MainWindow::saveDirSettings(const QString &dirPath)
{
    QSettings settings(QCoreApplication::applicationDirPath() + QStringLiteral("/config.ini"),
                       QSettings::IniFormat);
    settings.setValue(QStringLiteral("Settings/dirpath"), dirPath);
}

QString MainWindow::loadDirSettings() const
{
    QSettings settings(QCoreApplication::applicationDirPath() + QStringLiteral("/config.ini"),
                       QSettings::IniFormat);
    return settings.value(QStringLiteral("Settings/dirpath"), QString()).toString();
}

QString MainWindow::normalizePath(const QString &path) const
{
    return QDir::fromNativeSeparators(path.trimmed());
}

QString MainWindow::unlockDbPath(const QString &gameDir) const
{
    return gameDir + QStringLiteral("/Data/ui/unlockdb.xml");
}

QString MainWindow::backupDbPath(const QString &gameDir) const
{
    return gameDir + QStringLiteral("/Data/ui/filebackup/unlockdb.xml");
}

QString MainWindow::backupDirPath(const QString &gameDir) const
{
    return gameDir + QStringLiteral("/Data/ui/filebackup");
}

bool MainWindow::checkUnlockFile(const QString &gameDir, bool backup) const
{
    const QString path = backup ? backupDbPath(gameDir) : unlockDbPath(gameDir);
    return QFile::exists(path);
}

void MainWindow::refreshTableIfValid(const QString &gameDir)
{
    if (gameDir.isEmpty()) {
        return;
    }
    const QString xmlPath = unlockDbPath(gameDir);
    if (QFile::exists(xmlPath)) {
        showXmlFile(xmlPath);
        ui->statusBar->showMessage(tr("Loaded unlockdb.xml"), 3000);
    }
}

void MainWindow::showInfo(const QString &text)
{
    CustomDialog::showInformation(text, this);
}

void MainWindow::showWarn(const QString &text)
{
    CustomDialog::showWarning(text, this);
}

void MainWindow::on_pushButton_clicked()
{
    QString startDir = ui->lineEdit->text().trimmed();
    if (startDir.isEmpty()) {
        startDir = detectGameFolder();
    }
    if (startDir.isEmpty()) {
        startDir = QStringLiteral("./");
    }
    const QString dirPath = QFileDialog::getExistingDirectory(
        this, tr("Select game folder"), startDir,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dirPath.isEmpty()) {
        return;
    }

    ui->lineEdit->setText(QDir::toNativeSeparators(dirPath));
    saveDirSettings(ui->lineEdit->text());

    const QString normalized = normalizePath(dirPath);
    if (!checkUnlockFile(normalized)) {
        showWarn(tr("unlockdb.xml not found. Please verify the game folder."));
        return;
    }

    refreshTableIfValid(normalized);
    ui->statusBar->showMessage(tr("Game folder set"), 3000);
}

void MainWindow::on_pushButton_3_clicked()
{
    const QString gameDir = normalizePath(ui->lineEdit->text());
    if (!checkUnlockFile(gameDir)) {
        showWarn(tr("File not found. Please check the game folder."));
        return;
    }

    if (!ensureBackupDir(gameDir)) {
        showWarn(tr("Failed to create backup folder."));
        return;
    }

    removeIfExists(backupDbPath(gameDir));
    if (backupFile(unlockDbPath(gameDir), backupDirPath(gameDir))) {
        showInfo(tr("Backup completed."));
        ui->statusBar->showMessage(tr("Backup completed"), 3000);
    } else {
        showWarn(tr("Backup failed."));
    }
}

bool MainWindow::ensureBackupDir(const QString &gameDir)
{
    QDir dir;
    return dir.mkpath(backupDirPath(gameDir));
}

bool MainWindow::removeIfExists(const QString &filePath)
{
    if (!QFile::exists(filePath)) {
        return true;
    }
    return QFile::remove(filePath);
}

bool MainWindow::backupFile(const QString &src, const QString &dstDir)
{
    return QFile::copy(src, dstDir + QStringLiteral("/unlockdb.xml"));
}

void MainWindow::on_pushButton_2_clicked()
{
    const QString gameDir = normalizePath(ui->lineEdit->text());
    if (!checkUnlockFile(gameDir)) {
        showWarn(tr("File not found. Please check the game folder."));
        return;
    }

    if (!CustomDialog::showCustomDialog(
            tr("Please back up first! Continue unlocking?"), this)) {
        return;
    }

    const QString xmlPath = unlockDbPath(gameDir);
    editXmlFile(xmlPath);
    showXmlFile(xmlPath);
    showInfo(tr("Unlock completed!"));
    ui->statusBar->showMessage(tr("Unlock completed"), 4000);
}

void MainWindow::on_pushButton_4_clicked()
{
    const QString gameDir = normalizePath(ui->lineEdit->text());
    if (!checkUnlockFile(gameDir, true)) {
        showWarn(tr("Backup file not found."));
        return;
    }

    if (restoreFile(gameDir)) {
        showInfo(tr("Restore completed."));
        showXmlFile(unlockDbPath(gameDir));
        ui->statusBar->showMessage(tr("Restore completed"), 3000);
    } else {
        showWarn(tr("Restore failed."));
    }
}

bool MainWindow::restoreFile(const QString &gameDir)
{
    const QString dst = unlockDbPath(gameDir);
    removeIfExists(dst);
    return QFile::copy(backupDbPath(gameDir), dst);
}

void MainWindow::editXmlFile(const QString &fullFilePath)
{
    QFile file(fullFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        showWarn(tr("Failed to open file."));
        return;
    }

    QDomDocument doc;
    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;
    if (!doc.setContent(&file, &errorMsg, &errorLine, &errorColumn)) {
        file.close();
        showWarn(tr("XML parse failed: %1 (line %2)").arg(errorMsg).arg(errorLine));
        return;
    }
    file.close();

    const QDomNodeList unlocks = doc.elementsByTagName(QStringLiteral("Unlock"));
    for (int i = 0; i < unlocks.size(); ++i) {
        QDomElement element = unlocks.at(i).toElement();
        if (!element.isNull()) {
            element.setAttribute(QStringLiteral("state"),
                                 QStringLiteral("eUnlockState_Unlocked"));
        }
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        showWarn(tr("Failed to write file."));
        return;
    }

    QTextStream stream(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stream.setCodec("UTF-8");
#endif
    doc.save(stream, 4);
    file.close();
}

void MainWindow::showXmlFile(const QString &fullFilePath)
{
    if (!QFile::exists(fullFilePath)) {
        return;
    }

    if (!model) {
        model = new QStandardItemModel(this);
        ui->tableView->setModel(model);
    }

    const bool sorting = ui->tableView->isSortingEnabled();
    ui->tableView->setSortingEnabled(false);

    model->clear();
    model->setHorizontalHeaderLabels({tr("ID"), tr("Type"), tr("Category"), tr("State")});

    QFile file(fullFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QDomDocument doc;
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    const QDomNodeList unlocks = doc.elementsByTagName(QStringLiteral("Unlock"));
    for (int i = 0; i < unlocks.count(); ++i) {
        const QDomElement unlockElement = unlocks.at(i).toElement();
        const QString id = unlockElement.attribute(QStringLiteral("id"))
                               .replace(QStringLiteral("eUnlockItemId_"), QString());
        const QString type = unlockElement.attribute(QStringLiteral("type"))
                                 .replace(QStringLiteral("eUnlockType_"), QString());
        const QString category = unlockElement.attribute(QStringLiteral("category"))
                                     .replace(QStringLiteral("eUnlockCategory_"), QString());
        const QString state = stateToDisplay(unlockElement.attribute(QStringLiteral("state")));

        QList<QStandardItem *> rowItems;
        rowItems << new QStandardItem(id)
                 << new QStandardItem(type)
                 << new QStandardItem(category)
                 << new QStandardItem(state);
        for (QStandardItem *item : rowItems) {
            item->setEditable(false);
        }
        model->appendRow(rowItems);
    }

    ui->tableView->setColumnWidth(0, 220);
    ui->tableView->setColumnWidth(1, 160);
    ui->tableView->setColumnWidth(2, 160);
    ui->tableView->setSortingEnabled(sorting);
}
