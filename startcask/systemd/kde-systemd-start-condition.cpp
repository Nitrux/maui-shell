/*
    SPDX-FileCopyrightText: 2020 Henri Chain <henri.chain@enioka.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "../plasmaautostart/plasmaautostart.h"
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    // If invoked on gnome we should always return success
    // this is because a desktop file that has X-KDE-AutostartCondition
    // probably has an X-Gnome- equivalent and we only want one to run
    // this would match non systemd behaviour
    if (!qEnvironmentVariable("XDG_CURRENT_DESKTOP").split(QLatin1Char(':')).contains("kde", Qt::CaseInsensitive)) {
        return 0;
    }
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Checks start condition for a KDE systemd service"));
    parser.addHelpOption();
    QCommandLineOption option{QStringLiteral("condition"),
                              QStringLiteral("start condition, in the format 'rcfile:group:entry:default'."),
                              QStringLiteral("condition")};
    parser.addOption(option);
    parser.process(app);

    if (!parser.isSet(option)) {
        parser.showHelp(255);
    }

    if (PlasmaAutostart::isStartConditionMet(parser.value(option))) {
        return 0;
    } else {
        return 1;
    }
}
