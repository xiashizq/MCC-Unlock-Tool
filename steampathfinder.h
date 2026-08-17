#ifndef STEAMPATHFINDER_H
#define STEAMPATHFINDER_H

#include <QString>
#include <QStringList>

class SteamPathFinder
{
public:
    // Halo: The Master Chief Collection
    static constexpr const char *kMccAppId = "976730";

    static QString findHaloMcc();

private:
    static QString steamInstallPath();
    static QStringList steamLibraryRoots(const QString &steamRoot);
    static QStringList parseLibraryFoldersVdf(const QString &vdfPath);
    static QString installDirFromManifest(const QString &acfPath);
    static QString candidateGameDir(const QString &libraryRoot);
    static QString unescapeVdf(const QString &value);
};

#endif // STEAMPATHFINDER_H
