#include <QDebug>
#include <QProcess>

int runCommandSync(QString command, QStringList args) {
  QProcess process;

  process.start(command, args);
  process.waitForFinished();

  qDebug() << process.readAllStandardError();

  return process.exitCode();
}

int main() {
  qputenv("QT_QPA_PLATFORM", "wayland");
  qputenv("XDG_SESSION_TYPE", "wayland");

  QString command = "kwin_wayland";
  QStringList args;
  args << "--xwayland"
       << "--libinput"
       << "--exit-with-session=/usr/bin/cask";

  return runCommandSync(command, args);
}
