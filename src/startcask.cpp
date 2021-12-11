#include <QDebug>
#include <QProcess>
#include <QStandardPaths>
#include <QFile>
#include <QFileInfo>
#include <QDir>

int runCommandSync(QString command, QStringList args) {
//  QProcess process;

//  process.start(command, args);
//  process.waitForFinished();

//  qDebug() << process.readAllStandardError();


  QProcess p;
          p.setEnvironment(QProcess::systemEnvironment());
      p.setProcessChannelMode(QProcess::ForwardedChannels);
      p.start(command, args);
      //     qCDebug(PLASMA_STARTUP) << "started..." << program << args;
      p.waitForFinished(-1);
      if (p.exitCode()) {
          qWarning() << command << args << "exited with code" << p.exitCode();
      }
      qWarning() << "STARTCASK FINISHED" << p.exitStatus() << p.exitCode();
      return p.exitCode();


//  QFile log("home/camilo/mauilog.txt");
 //// log.open(QFile::WriteOnly | QFile::Text);
 // log.write(process.readAll());
 // log.close();
//  return process.exitCode();
}


bool isShellVariable(const QByteArray &name) {
    return name == "_" || name.startsWith("SHLVL");
}

bool isSessionVariable(const QByteArray &name) {
    // Check is variable is specific to session.
    return name == "DISPLAY" || name == "XAUTHORITY" ||
           name == "WAYLAND_DISPLAY" || name == "WAYLAND_SOCKET" ||
           name.startsWith("XDG_");
}


void setEnvironmentVariable(const QByteArray &name, const QByteArray &value) {
    if (qgetenv(name) != value) {
        //        qCDebug(PLASMA_STARTUP) << "setting..." << env.left(idx) << env.mid(idx+1) << "was" << qgetenv(env.left(idx));
        qputenv(name, value);
    }
}


void sourceFiles(const QStringList &files)
{
    QStringList filteredFiles;
    std::copy_if(files.begin(), files.end(), std::back_inserter(filteredFiles), [](const QString &i) {
        return QFileInfo(i).isReadable();
    });

    if (filteredFiles.isEmpty())
        return;

//    filteredFiles.prepend(QStringLiteral(CMAKE_INSTALL_FULL_LIBEXECDIR "/plasma-sourceenv.sh"));

    QProcess p;
    p.start(QStringLiteral("/bin/sh"), filteredFiles);
    p.waitForFinished(-1);

    const auto fullEnv = p.readAllStandardOutput();
    auto envs = fullEnv.split('\0');

    for (auto &env : envs) {
        const int idx = env.indexOf('=');
        if (Q_UNLIKELY(idx <= 0)) {
            continue;
        }

        const auto name = env.left(idx);
        if (isShellVariable(name)) {
            continue;
        }
        setEnvironmentVariable(name, env.mid(idx + 1));
    }
}



void runEnvironmentScripts()
{
    QStringList scripts;
    auto locations = QStandardPaths::standardLocations(QStandardPaths::GenericConfigLocation);

    //`standardLocations()` returns locations sorted by "order of priority". We iterate in reverse
    // order so that high-priority scripts are sourced last and their modifications take precedence.
    for (auto loc = locations.crbegin(); loc != locations.crend(); loc++) {
        QDir dir(*loc);
        if (!dir.cd(QStringLiteral("./plasma-workspace/env"))) {
            // Skip location if plasma-workspace/env subdirectory does not exist
            continue;
        }
        const auto dirScripts = dir.entryInfoList({QStringLiteral("*.sh")}, QDir::Files, QDir::Name);
        for (const auto &script : dirScripts) {
            scripts << script.absoluteFilePath();
        }
    }
    sourceFiles(scripts);
}

int main()
{

 qputenv("QT_QPA_PLATFORM", "eglfs");

  qputenv("XDG_SESSION_TYPE", "wayland");
  qputenv("XDG_CURRENT_DESKTOP", "Cask");
  qputenv("XDG_CONFIG_DIRS", "/home/camilo/.config/kdedefaults:/etc/xdg");
  qputenv("XDG_SEAT_PATH", "/org/freedesktop/DisplayManager/Seat0");
  qputenv("XDG_SESSION_PATH", "/org/freedesktop/DisplayManager/Session0");

  qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");

  qputenv("QT_QUICK_CONTROLS_STYLE", "maui-style");

//  QString command = "/home/camilo/tests/grefsen/grefsen";
  QString command = "cask";
  QStringList args;
//  args   << "-plugin libinput";
//       << "--exit-with-session=/usr/bin/cask";
//  runEnvironmentScripts();
  return runCommandSync(command, args);
}
