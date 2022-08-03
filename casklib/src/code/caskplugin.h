#pragma once

#include <QDir>
#include <QQmlExtensionPlugin>

class CaskPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri) override;
private:
    void initializeEngine(QQmlEngine *engine, const char *uri) override;

    QString resolveFileUrl(const QString &filePath) const
    {
#ifdef QUICK_COMPILER
        return QStringLiteral("qrc:/maui/cask/") + filePath;
#else
        return baseUrl().toString() + QLatin1Char('/') + filePath;
#endif
    }
};
