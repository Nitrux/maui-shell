/*
    SPDX-FileCopyrightText: 2019 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "../autostart.h"
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    AutoStart as;

    QTextStream cout(stdout);
    auto printPhase = [&cout, &as](int phase) -> bool {
        AutoStart asN(as);
        asN.setPhase(phase);
        cout << "phase: " << phase << '\n';
        bool foundThings = true;
        for (const auto &asi : asN.startList()) {
            foundThings = false;
            cout << "- " << asi.name << ' ' << asi.service;
            if (!asi.startAfter.isEmpty())
                cout << ", startAfter:" << asi.startAfter;
            cout << '\n';
        }
        cout << '\n';
        return !foundThings;
    };

    printPhase(0);
    printPhase(1);
    printPhase(2);
    return 0;
}
