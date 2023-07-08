#pragma once

#include <MauiKit4/Core/mauilist.h>

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
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)

public:
    RecentFilesModel(QObject * parent = nullptr);

    const FMH::MODEL_LIST &items() const override final;

    QUrl url() const;
    QStringList filters() const;

    void componentComplete() override final;

    QStringList urls() const;

    int limit() const;

public Q_SLOTS:
    void setUrl(QUrl url);

    void setFilters(QStringList filters);

    void setLimit(int limit);

Q_SIGNALS:
    void urlChanged(QUrl url);

    void filtersChanged(QStringList filters);

    void urlsChanged();

    void limitChanged(int limit);

private:
    FMH::MODEL_LIST m_list;
    FMH::FileLoader * m_loader;
    QFileSystemWatcher *m_watcher;
    void setList();

    QUrl m_url;
    QStringList m_filters;
    QStringList m_urls;
    int m_limit = 6;
};
