#include "recentfilesmodel.h"

#include <MauiKit/FileBrowsing/fileloader.h>
#include <MauiKit/FileBrowsing/fmstatic.h>

#include <QFileSystemWatcher>
#include <QDebug>

RecentFilesModel::RecentFilesModel(QObject *parent) :
  MauiList(parent)
, m_loader(new FMH::FileLoader)
, m_watcher(new QFileSystemWatcher(this))
{
//  connect(m_loader, &FMH::FileLoader::itemsReady, [&](FMH::MODEL_LIST items)
//  {
//    emit preItemsAppended(items.size());
//    this->m_list << items;
//    emit postItemAppended();
//  });

    connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &RecentFilesModel::setList);
}


const FMH::MODEL_LIST &RecentFilesModel::items() const
{
  return m_list;
}

QUrl RecentFilesModel::url() const
{
  return m_url;
}

void RecentFilesModel::setUrl(QUrl url)
{
  if (m_url == url)
    return;

  m_url = url;
  if(!m_watcher->directories().isEmpty())
  {
      m_watcher->removePaths(m_watcher->directories());
  }
  m_watcher->addPath(m_url.toLocalFile());
  emit urlChanged(m_url);
}

void RecentFilesModel::setFilters(QStringList filters)
{
  if (m_filters == filters)
    return;

  m_filters = filters;
  emit filtersChanged(m_filters);
}

void RecentFilesModel::setList()
{
  if (!m_url.isLocalFile () || !m_url.isValid () || m_url.isEmpty ())
    return;

//  m_loader->informer = &FMH::getFileInfoModel;
//  m_loader->requestPath({m_url}, true, m_filters.isEmpty () ? QStringList () : m_filters, QDir::Files, 50);

  QDir dir(m_url.toLocalFile ());
  dir.setNameFilters (m_filters);
  dir.setFilter (QDir::Files);
  dir.setSorting (QDir::Time);
  int i = 0;

  this->m_list.clear();
  emit this->preListChanged ();
  const auto urls = dir.entryInfoList ();
  for(const auto &url : urls)
    {
      if(i >= 6)
        break;
      qDebug() << "RECENT:" << url.filePath () << dir.path ();
      m_urls << QUrl::fromLocalFile (url.filePath ()).toString();
      m_list << FMStatic::getFileInfoModel (QUrl::fromLocalFile (url.filePath ()));
      i++;
    }
  emit postListChanged ();
  emit urlsChanged();
}


void RecentFilesModel::componentComplete()
{
  connect(this, &RecentFilesModel::urlChanged, this, &RecentFilesModel::setList);
  connect(this, &RecentFilesModel::filtersChanged, this, &RecentFilesModel::setList);
  setList ();
}

QStringList RecentFilesModel::urls() const
{
    return m_urls;
}

QStringList RecentFilesModel::filters() const
{
    return m_filters;
}
