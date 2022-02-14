#pragma once

#include <QObject>
#include <QQmlExtensionPlugin>

class CaskPolkitPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)
public:
    void registerTypes(const char *uri) override;

    QString resolveFileUrl(const QString &filePath) const
    {
#ifdef QUICK_COMPILER
        return QStringLiteral("qrc:/cask/polkit/") + filePath;
#else
        return baseUrl().toString() + QLatin1Char('/') + filePath;
#endif
    }
};


