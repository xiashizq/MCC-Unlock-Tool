#ifndef TRANSLATIONMANAGER_H
#define TRANSLATIONMANAGER_H

#include <QObject>
#include <QString>
#include <QTranslator>

class QApplication;

class TranslationManager : public QObject
{
    Q_OBJECT

public:
    static TranslationManager &instance();

    void initialize(QApplication *app);
    QString currentLanguage() const { return m_language; }
    void setLanguage(const QString &language);
    QStringList availableLanguages() const;

signals:
    void languageChanged(const QString &language);

private:
    explicit TranslationManager(QObject *parent = nullptr);
    bool loadTranslator(const QString &language);

    QApplication *m_app = nullptr;
    QTranslator m_appTranslator;
    QString m_language;
};

#endif // TRANSLATIONMANAGER_H
