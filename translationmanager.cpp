#include "translationmanager.h"

#include <QApplication>
#include <QCoreApplication>
#include <QSettings>

TranslationManager &TranslationManager::instance()
{
    static TranslationManager mgr;
    return mgr;
}

TranslationManager::TranslationManager(QObject *parent)
    : QObject(parent)
{
}

void TranslationManager::initialize(QApplication *app)
{
    m_app = app;

    QSettings settings(QCoreApplication::applicationDirPath() + QStringLiteral("/config.ini"),
                       QSettings::IniFormat);
    const QString saved = settings.value(QStringLiteral("Settings/language"),
                                         QStringLiteral("en")).toString();
    setLanguage(saved.isEmpty() ? QStringLiteral("en") : saved);
}

QStringList TranslationManager::availableLanguages() const
{
    return {QStringLiteral("en"), QStringLiteral("zh_CN")};
}

void TranslationManager::setLanguage(const QString &language)
{
    QString lang = language;
    if (lang != QLatin1String("zh_CN")) {
        lang = QStringLiteral("en");
    }

    if (m_language == lang && !m_language.isEmpty()) {
        return;
    }

    if (m_app) {
        m_app->removeTranslator(&m_appTranslator);
    }

    m_language = lang;
    loadTranslator(lang);

    QSettings settings(QCoreApplication::applicationDirPath() + QStringLiteral("/config.ini"),
                       QSettings::IniFormat);
    settings.setValue(QStringLiteral("Settings/language"), m_language);

    emit languageChanged(m_language);
}

bool TranslationManager::loadTranslator(const QString &language)
{
    if (language == QLatin1String("en") || !m_app) {
        return true;
    }

    const QString path = QStringLiteral(":/i18n/halounlocktool_%1.qm").arg(language);
    if (m_appTranslator.load(path)) {
        m_app->installTranslator(&m_appTranslator);
        return true;
    }
    return false;
}
