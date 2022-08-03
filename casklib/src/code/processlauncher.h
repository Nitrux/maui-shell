#pragma once

#include <QObject>
#include <QProcess>

class WaylandProcessLauncher : public QObject
{
    Q_OBJECT

public:
    explicit WaylandProcessLauncher(QObject *parent = nullptr);
    ~WaylandProcessLauncher();
    Q_INVOKABLE void launch(const QString &program);
    Q_INVOKABLE void launchApp(const QString &program);
protected slots:
    void onError(QProcess::ProcessError error);
    void onStateChanged(QProcess::ProcessState state);
};

