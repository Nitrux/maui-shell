/*
    SPDX-FileCopyrightText: 2019 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "config-startcask.h"
#include <QProcessEnvironment>
//#include "kcheckrunning/kcheckrunning.h"
//#include <ksplashinterface.h>
#include <optional>

extern QTextStream out;

void sigtermHandler(int signalNumber);
QStringList allServices(const QLatin1String &prefix);
int runSync(const QString &program, const QStringList &args, const QStringList &env = {});
void sourceFiles(const QStringList &files);
void messageBox(const QString &text);

void createConfigDirectory();
void runStartupConfig();
void setupCursor();
std::optional<QProcessEnvironment> getSystemdEnvironment();
void importSystemdEnvrionment();
void runEnvironmentScripts();
void setupCaskEnvironment();
void cleanupPlasmaEnvironment(const std::optional<QProcessEnvironment> &oldSystemdEnvironment);
bool syncDBusEnvironment();
void setupFontDpi();
//QProcess *setupKSplash();

bool startCaskSession();

void waitForKonqi();

void playStartupSound(QObject *parent);

void gentleTermination(QProcess *process);

struct KillBeforeDeleter {
    static inline void cleanup(QProcess *pointer)
    {
        if (pointer) {
            gentleTermination(pointer);
        }
        delete pointer;
    }
};
