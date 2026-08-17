#include "steampathfinder.h"

#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QSettings>
#include <QTextStream>

namespace {
bool hasUnlockDb(const QString &gameDir)
{
    return QFile::exists(gameDir + QLatin1String("/Data/ui/unlockdb.xml"));
}

QString nativeClean(const QString &path)
{
    return QDir::toNativeSeparators(QDir::cleanPath(path));
}
} // namespace

QString SteamPathFinder::unescapeVdf(const QString &value)
{
    QString out = value;
    out.replace(QLatin1String("\\\\"), QLatin1String("\\"));
    out.replace(QLatin1String("\\/"), QLatin1String("/"));
    return QDir::fromNativeSeparators(out.trimmed());
}

QString SteamPathFinder::steamInstallPath()
{
#ifdef Q_OS_WIN
    const QStringList keys = {
        QStringLiteral("HKEY_CURRENT_USER\\Software\\Valve\\Steam"),
        QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Valve\\Steam"),
        QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\Valve\\Steam"),
    };
    const QStringList valueNames = {
        QStringLiteral("SteamPath"),
        QStringLiteral("InstallPath"),
    };

    for (const QString &key : keys) {
        QSettings settings(key, QSettings::NativeFormat);
        for (const QString &name : valueNames) {
            const QString path = settings.value(name).toString().trimmed();
            if (!path.isEmpty() && QDir(path).exists()) {
                return QDir::fromNativeSeparators(path);
            }
        }
    }
#endif

    const QStringList fallbacks = {
        QStringLiteral("C:/Program Files (x86)/Steam"),
        QStringLiteral("C:/Program Files/Steam"),
    };
    for (const QString &path : fallbacks) {
        if (QDir(path).exists()) {
            return path;
        }
    }
    return {};
}

QStringList SteamPathFinder::parseLibraryFoldersVdf(const QString &vdfPath)
{
    QFile file(vdfPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }

    const QString content = QString::fromUtf8(file.readAll());
    QStringList roots;

    QRegularExpression pathRe(QStringLiteral("\"path\"\\s*\"([^\"]+)\""));
    auto it = pathRe.globalMatch(content);
    while (it.hasNext()) {
        const QString root = unescapeVdf(it.next().captured(1));
        if (!root.isEmpty() && QDir(root).exists() && !roots.contains(root, Qt::CaseInsensitive)) {
            roots.append(root);
        }
    }

    if (roots.isEmpty()) {
        QRegularExpression oldRe(QStringLiteral("\"\\d+\"\\s*\"([A-Za-z]:\\\\[^\"]+)\""));
        auto oldIt = oldRe.globalMatch(content);
        while (oldIt.hasNext()) {
            const QString root = unescapeVdf(oldIt.next().captured(1));
            if (!root.isEmpty() && QDir(root).exists() && !roots.contains(root, Qt::CaseInsensitive)) {
                roots.append(root);
            }
        }
    }

    return roots;
}

QStringList SteamPathFinder::steamLibraryRoots(const QString &steamRoot)
{
    QStringList roots;
    if (!steamRoot.isEmpty()) {
        roots.append(steamRoot);
    }

    const QStringList vdfCandidates = {
        steamRoot + QLatin1String("/steamapps/libraryfolders.vdf"),
        steamRoot + QLatin1String("/config/libraryfolders.vdf"),
    };
    for (const QString &vdf : vdfCandidates) {
        for (const QString &root : parseLibraryFoldersVdf(vdf)) {
            if (!roots.contains(root, Qt::CaseInsensitive)) {
                roots.append(root);
            }
        }
    }
    return roots;
}

QString SteamPathFinder::installDirFromManifest(const QString &acfPath)
{
    QFile file(acfPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }

    QTextStream stream(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stream.setCodec("UTF-8");
#endif
    const QString content = stream.readAll();
    QRegularExpression dirRe(QStringLiteral("\"installdir\"\\s*\"([^\"]+)\""));
    const auto match = dirRe.match(content);
    if (match.hasMatch()) {
        return unescapeVdf(match.captured(1));
    }
    return {};
}

QString SteamPathFinder::candidateGameDir(const QString &libraryRoot)
{
    const QString steamapps = libraryRoot + QLatin1String("/steamapps");
    const QString acf = steamapps + QLatin1String("/appmanifest_")
            + QLatin1String(kMccAppId) + QLatin1String(".acf");

    QString installDir = installDirFromManifest(acf);
    if (installDir.isEmpty()) {
        installDir = QStringLiteral("Halo The Master Chief Collection");
    }

    const QString gameDir = steamapps + QLatin1String("/common/") + installDir;
    if (hasUnlockDb(gameDir)) {
        return nativeClean(gameDir);
    }

    const QString fallback = steamapps + QLatin1String("/common/Halo The Master Chief Collection");
    if (QDir::cleanPath(gameDir).compare(QDir::cleanPath(fallback), Qt::CaseInsensitive) != 0
        && hasUnlockDb(fallback)) {
        return nativeClean(fallback);
    }
    return {};
}

QString SteamPathFinder::findHaloMcc()
{
    const QString steamRoot = steamInstallPath();
    if (steamRoot.isEmpty()) {
        return {};
    }

    for (const QString &library : steamLibraryRoots(steamRoot)) {
        const QString gameDir = candidateGameDir(library);
        if (!gameDir.isEmpty()) {
            return gameDir;
        }
    }
    return {};
}
