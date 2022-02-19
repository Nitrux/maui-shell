/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2006 Aaron Seigo <aseigo@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDELIBS_KAUTOSTART_H
#define KDELIBS_KAUTOSTART_H

#include <QObject>
#include <QStringList>

class KDesktopFile;

/*
 * This class was imported from KService at commit c2cedaaeba0a53939c96a1273ab92ed1d6ca7bcf
 */
class PlasmaAutostart : public QObject
{
    Q_OBJECT

public:
    /**
     * Creates a new KAutostart object that represents the autostart
     * service "entryName". If the service already exists in the system
     * then the values associated with that service, such as the executable
     * command, will be loaded as well.
     *
     * Note that unless this service is explicitly set to autostart,
     * simply creating a KAutostart object will not result in the
     * service being autostarted on next log in.
     *
     * If no such service is already registered and the command to be
     * executed on startup is not the same as entryName, then you will want
     * to set the associated command with setExec(const QString&)
     * @see setExec
     * @param entryName the name used to identify the service. If none is
     *        provided then it uses the name registered with KAboutData.
     * @param parent QObject
     */
    explicit PlasmaAutostart(const QString &entryName = QString(), QObject *parent = nullptr);
    ~PlasmaAutostart();

    /**
     * Flags for each of the conditions that may affect whether or not
     * a service actually autostarted on login
     * @see Conditions
     */
    enum Condition {
        /**
         * no conditions set
         */
        NoConditions = 0x0,
        /**
         * an executable that is checked for existence by name
         */
        CheckCommand = 0x1,
        /**
         * autostart condition will be checked too (KDE-specific)
         * @since 4.3
         */
        CheckCondition = 0x2,
        /**
         * all necessary conditions will be checked
         * @since 4.3
         */
        CheckAll = 0xff,
    };
    /**
     * Stores a combination of #Condition values.
     */
    Q_DECLARE_FLAGS(Conditions, Condition)

    /**
     * Enumerates the various autostart phases that occur during start-up.
     */
    enum StartPhase {
        /**
         * the essential desktop services such as panels and window managers
         */
        BaseDesktop = 0,
        /**
         * services that should be available before most interactive
         * applications start but that aren't part of the base desktop.
         * This would include things such as clipboard managers and
         * mouse gesture tools.
         */
        DesktopServices = 1,
        /**
         * everything else that doesn't belong in the above two categories,
         * including most system tray applications, system monitors and
         * interactive applications
         */
        Applications = 2,
    };

    /**
     * Sets the given exec to start automatically at login
     * @param autostart will register with the autostart facility when true
     *        and deregister when false
     * @see autostarts()
     */
    void setAutostarts(bool autostart);

    /**
     * Returns whether or not the service represented by entryName in the
     * autostart system is set to autostart at login or not
     * @param environment if provided the check will be performed as if
     *        being loaded in that environment
     * @param check autostart conditions to check for (see commandToCheck())
     * @see setAutostarts()
     */
    bool autostarts(const QString &environment = QString(), Conditions check = NoConditions) const;

    /**
     * Returns the associated command for this autostart service
     * @see setCommand()
     */
    QString command() const;
    /**
     * Set the associated command for this autostart service
     * @see command()
     */
    void setCommand(const QString &command);

    /**
     * Checks whether or not a service by the given name @p entryName is registered
     * with the autostart system. Does not check whether or not it is
     * set to actually autostart or not.
     * @param entryName the name of the service to check for
     */
    static bool isServiceRegistered(const QString &entryName);

    /**
     * Returns the autostart phase this service is started in.
     *
     * Note that this is KDE specific and may not work in other
     * environments.
     *
     * @see StartPhase, setStartPhase()
     */
    StartPhase startPhase() const;

    /**
     * Returns the list of environments (e.g. "KDE") this service is allowed
     * to start in. Use checkAllowedEnvironment() or autostarts() for actual
     * checks.
     *
     * This does not take other autostart conditions
     * into account. If any environment is added to the allowed environments
     * list, then only those environments will be allowed to
     * autoload the service. It is not allowed to specify both allowed and excluded
     * environments at the same time.
     * @see setAllowedEnvironments()
     */
    QStringList allowedEnvironments() const;

    /**
     * Returns the list of environments this service is explicitly not
     * allowed to start in. Use checkAllowedEnvironment() or autostarts() for actual
     * checks.
     *
     * This does not take other autostart conditions
     * such as into account. It is not allowed to specify both allowed and excluded
     * environments at the same time.
     * @see setExcludedEnvironments()
     */
    QStringList excludedEnvironments() const;

    /**
     * Returns the name of another service that should be autostarted
     * before this one (if that service would be autostarted).
     * @internal
     */
    QString startAfter() const;

    /**
     * Checks whether autostart is allowed in the given environment,
     * depending on allowedEnvironments() and excludedEnvironments().
     */
    bool checkAllowedEnvironment(const QString &environment) const;

    /**
     * Checks that a given autostart configuration condition is met.
     * @param condition: config in the format "rcfile:group:entry:default"
     */
    static bool isStartConditionMet(const QString &condition);

private:
    bool checkStartCondition() const;
    void copyIfNeeded();
    QString name;
    KDesktopFile *df;
    bool copyIfNeededChecked;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PlasmaAutostart::Conditions)
#endif
