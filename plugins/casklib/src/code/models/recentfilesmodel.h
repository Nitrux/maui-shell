#ifndef RECENTFILESMODEL_H
#define RECENTFILESMODEL_H

#include <MauiKit/Core/mauilist.h>

#include <QObject>

class QFileSystemWatcher;
namespace FMH
{
class FileLoader;
}

class RecentFilesModel : public MauiList
{
    Q_OBJECT
    Q_PROPERTY(QUrl url WRITE setUrl READ url NOTIFY urlChanged)
    Q_PROPERTY(QStringList filters WRITE setFilters READ filters NOTIFY filtersChanged)
    Q_PROPERTY(QStringList urls READ urls NOTIFY urlsChanged FINAL)

public:
    RecentFilesModel(QObject * parent = nullptr);

    const FMH::MODEL_LIST &items() const override final;

    QUrl url() const;
    QStringList filters() const;

    void componentComplete() override final;

    QStringList urls() const;

public slots:
    void setUrl(QUrl url);

    void setFilters(QStringList filters);

signals:
    void urlChanged(QUrl url);

    void filtersChanged(QStringList filters);

    void urlsChanged();

private:
    FMH::MODEL_LIST m_list;
    FMH::FileLoader * m_loader;
    QFileSystemWatcher *m_watcher;
    void setList();

    QUrl m_url;
    QStringList m_filters;
    QStringList m_urls;
};

#endif // RECENTFILESMODEL_H
