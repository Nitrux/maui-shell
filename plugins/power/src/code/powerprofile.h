#ifndef POWERPROFILE_H
#define POWERPROFILE_H

#include <QObject>

class PowerProfile : public QObject
{
    Q_OBJECT
    bool m_acPluggedIn;

public:
    explicit PowerProfile(QObject *parent = nullptr);

public slots:

signals:

};

#endif // POWERPROFILE_H
