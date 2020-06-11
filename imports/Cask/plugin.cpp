#include <QDir>
#include <QJSEngine>
#include <QCoreApplication>
#include <QLoggingCategory>
#include <QQmlEngine>
#include <QQmlExtensionPlugin>
#include <QtQml>

#include "models/appsmodel.h"

Q_LOGGING_CATEGORY(lcRegistration, "cask.registration")

class CaskPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    CaskPlugin() : QQmlExtensionPlugin() { }
    QUrl fromBase(QString url) {
           return QDir::cleanPath(baseUrl().toString()+QDir::separator()+url);
       };
    void registerTypes(const char *uri)
    {
        Q_ASSERT(uri == QLatin1String("org.maui.cask"));
        qmlRegisterType(fromBase(QStringLiteral("Cask.qml")), uri, 1, 0, "Cask");
        qmlRegisterType<AppsModel>(uri, 1, 0, "AppsModel");
     }
};

QT_END_NAMESPACE

#include "plugin.moc"
