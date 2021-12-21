
#ifndef NOTIFICATIONIMAGESSTORAGE_H
#define NOTIFICATIONIMAGESSTORAGE_H

#include <QHash>
#include <QPixmap>

struct NotificationImage {
    QPixmap image;
    QString iconName;
    QString entryIconName;
};

class NotificationImagesStorage
{
public:
    NotificationImagesStorage();
    ~NotificationImagesStorage();

    NotificationImage *get(uint id) const;

    void add(uint id, NotificationImage *image);
    void remove(uint id);

    static NotificationImagesStorage *instance();

private:
    QHash<uint, NotificationImage *> m_images;
};

#endif // NOTIFICATIONIMAGESSTORAGE_H
