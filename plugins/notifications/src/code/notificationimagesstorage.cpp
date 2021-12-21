
#include <QGlobalStatic>

#include "notificationimagesstorage.h"

Q_GLOBAL_STATIC(NotificationImagesStorage, globalStorage)

NotificationImagesStorage::NotificationImagesStorage()
{
}

NotificationImagesStorage::~NotificationImagesStorage()
{
    qDeleteAll(m_images);
}

NotificationImage *NotificationImagesStorage::get(uint id) const
{
    return m_images.value(id);
}

void NotificationImagesStorage::add(uint id, NotificationImage *image)
{
    remove(id);
    m_images[id] = image;
}

void NotificationImagesStorage::remove(uint id)
{
    if (m_images.contains(id))
        delete m_images.take(id);
}

NotificationImagesStorage *NotificationImagesStorage::instance()
{
    return globalStorage;
}
