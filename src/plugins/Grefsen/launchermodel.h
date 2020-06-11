#ifndef LAUNCHERMODEL_H
#define LAUNCHERMODEL_H

#include <QDomElement>
#include <QJSValue>
#include <QObject>

class XdgDesktopFile;

class LauncherModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QJSValue allApplications READ allApplications NOTIFY applicationsChanged)
    Q_PROPERTY(QJSValue applicationMenu READ applicationMenu NOTIFY applicationsChanged)
    Q_PROPERTY(QString substringFilter READ substringFilter WRITE setSubstringFilter NOTIFY substringFilterChanged)


public:
    explicit LauncherModel(QJSEngine *engine, QObject *parent = 0);
    QJSValue allApplications() { return m_allApps; }
    QJSValue applicationMenu() { return m_list.property(QStringLiteral("items")); }

    QString substringFilter() const { return m_substringFilter; }
    void setSubstringFilter(QString substringFilter);

signals:
    void applicationsChanged();
    void substringFilterChanged();
    void execFailed(QString error);

public slots:
    void reset();
    void select(QJSValue sel);
    void exec(QString desktopFilePath);
    void openSubmenu(QString title);


protected:
    void build(QJSValue in, const QDomElement& xml);
    void appendMenu(QJSValue in, const QDomElement& xml);
    void appendApp(QJSValue in, const QDomElement &xml);
    QJSValue findFirst(QString key, QString value, QJSValue array);
    QJSValue findSubstring(QString key, QString substr);

protected:
//    static QList<XdgDesktopFile *> m_allFiles;
    QJSEngine *m_engine;
    QDomElement m_dom;
    QJSValue m_root;
    QJSValue m_list;
    QJSValue m_allApps;
    int m_allAppsCount = 0;
    QString m_substringFilter;
};

#endif // LAUNCHERMODEL_H
