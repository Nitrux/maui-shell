#pragma once

#include <QObject>
#include <QQmlExtensionPlugin>

class CaskPolkitPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri) override;

private:
    QUrl componentUrl(const QString &fileName) const;

    QString resolveFileUrl(const QString &filePath) const
    {
        return baseUrl().toString() + QLatin1Char('/') + filePath;
    }
};


