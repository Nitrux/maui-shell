#pragma once
#include <QObject>

class InhibitionControl : public QObject
{
    Q_OBJECT
public:
    explicit InhibitionControl(QObject *parent = nullptr);

signals:

};

