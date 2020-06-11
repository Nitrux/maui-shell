#ifndef CASK_PLUGIN
#define CASKPLUGIN

#include <QDir>
#include <QJSEngine>
#include <QCoreApplication>
#include <QLoggingCategory>
#include <QQmlEngine>
#include <QQmlExtensionPlugin>
#include <QtQml>

#include "models/appsmodel.h"

class CaskPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    QUrl fromBase(QString url)
    {
           return QDir::cleanPath(baseUrl().toString()+QDir::separator()+url);
       };

    static void registerTypes()
      {
          static CaskPlugin instance;
          instance.registerTypes("org.maui.cask");
      }

    void registerTypes(const char *uri) override;
};
#endif
