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
    QUrl componentUrl(const QString &fileName) const;

    void initializeEngine(QQmlEngine *engine, const char *uri) override;

    QString resolveFileUrl(const QString &filePath) const
    {
        return baseUrl().toString() + QLatin1Char('/') + filePath;
    }
};
