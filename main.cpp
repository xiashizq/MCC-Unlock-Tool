#include "mainwindow.h"
#include "translationmanager.h"

#include <QApplication>
#include <QCoreApplication>
#include <QFont>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QScreen>

static QString resolveUiFontFamily()
{
    // Prefer Microsoft YaHei UI; also accept localized family names on Chinese Windows
    const QStringList candidates = {
        QStringLiteral("Microsoft YaHei UI"),
        QString::fromUtf8("\xe5\xbe\xae\xe8\xbd\xaf\xe9\x9b\x85\xe9\xbb\x91 UI"),
        QStringLiteral("Microsoft YaHei"),
        QString::fromUtf8("\xe5\xbe\xae\xe8\xbd\xaf\xe9\x9b\x85\xe9\xbb\x91"),
    };

    const QStringList families = QFontDatabase().families();
    for (const QString &name : candidates) {
        for (const QString &family : families) {
            if (family.compare(name, Qt::CaseInsensitive) == 0) {
                return family;
            }
        }
    }
    return QStringLiteral("Microsoft YaHei UI");
}

static QFont createAppFont()
{
    // Match Windows Chinese UI default size (YaHei UI @ 9pt)
    QFont font(resolveUiFontFamily(), 9);
    font.setStyleHint(QFont::SansSerif, QFont::PreferAntialias);
    // NoHinting improves CJK smoothness under Qt5
    font.setHintingPreference(QFont::PreferNoHinting);
    return font;
}

int main(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    // FreeType renders Microsoft YaHei much more cleanly than the default GDI path on Qt5
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "windows:fontengine=freetype");
    }
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#endif

    QApplication a(argc, argv);
    QApplication::setApplicationName(QStringLiteral("HaloUnlockTool"));
    QApplication::setOrganizationName(QStringLiteral("MumianYouji"));
    QApplication::setApplicationVersion(QStringLiteral("1.3"));
    // Native Windows style — Fusion makes YaHei look thinner
    a.setFont(createAppFont());

    TranslationManager::instance().initialize(&a);

    MainWindow w;

    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        const QRect avail = screen->availableGeometry();
        const int wHint = qMin(960, static_cast<int>(avail.width() * 0.72));
        const int hHint = qMin(560, static_cast<int>(avail.height() * 0.70));
        w.resize(wHint, hHint);
        w.setMinimumSize(720, 420);
        const QPoint center = avail.center() - QPoint(w.width() / 2, w.height() / 2);
        w.move(center);
    }

    w.show();
    return a.exec();
}
