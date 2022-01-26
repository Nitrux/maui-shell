#include "batteryinfo.h"

#include <QDBusConnectionInterface>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusPendingCallWatcher>
#include <QDBusReply>

#include <QDebug>

// solid specific includes
#include <solid/device.h>
#include <solid/deviceinterface.h>
#include <solid/devicenotifier.h>

static const char SOLID_POWERMANAGEMENT_SERVICE[] = "org.kde.Solid.PowerManagement";

BatteryInfo::BatteryInfo(QObject *parent)
    : QObject{parent}
    ,m_batteries(new BatteryModel(this))
    ,m_primaryBattery(nullptr)
{
    this->init();
}

BatteryModel::BatteryModel(QObject *parent) : QAbstractListModel(parent)
{

}

BatteryModel::~BatteryModel()
{
    qDeleteAll(m_batteries);
}

int BatteryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_batteries.size();
}

QVariant BatteryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto battery = m_batteries[index.row()];
    switch(role)
    {
    case 0: return QVariant::fromValue(battery);
    }

    return QVariant();

}

QHash<int, QByteArray> BatteryModel::roleNames() const
{
    return QHash<int, QByteArray> {{0, "Battery"}};
}

void BatteryModel::addBattery(Battery *battery)
{
    const auto index = m_batteries.size();
    this->beginInsertRows(QModelIndex(), index, index);
    m_batteries << battery;
    this->endInsertRows();
    emit this->countChanged();
}

void BatteryModel::removeBattery(Battery *battery)
{
    Q_UNUSED(battery)
}

void BatteryModel::removeBattery(const QString &udi)
{
    const auto index = indexOf(udi);
    if(index < 0 || index >= m_batteries.count())
    {
        return;
    }

    this->beginRemoveRows(QModelIndex(), index, index);
    m_batteries.remove(index);
    this->endRemoveRows();
    emit this->countChanged();
}

bool BatteryModel::contains(const QString &udi) const
{
    return indexOf(udi) >= 0;
}

int BatteryModel::indexOf(const QString &udi) const
{
    const auto it = std::find_if(m_batteries.constBegin(), m_batteries.constEnd(), [udi]( Battery * battery) -> bool
    {
            return   battery->udi() == udi;
});

    if (it != m_batteries.constEnd())
        return (std::distance(m_batteries.constBegin(), it));
    else
        return -1;
}

Battery *BatteryModel::battery(const QString &udi)
{
    const auto index = indexOf(udi);
    if(index < 0 || index >= m_batteries.count())
    {
        return nullptr;
    }

    return m_batteries[index];
}

BatteryModel *BatteryInfo::batteries() const
{
    return m_batteries;
}

bool BatteryInfo::hasBatteries() const
{
    return m_hasBatteries;
}

Battery *BatteryInfo::primaryBattery() const
{
    return m_primaryBattery;
}

bool BatteryInfo::acPluggedIn() const
{
    return m_acPluggedIn;
}

void BatteryInfo::init()
{
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceAdded, this, &BatteryInfo::deviceAdded);
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceRemoved, this, &BatteryInfo::deviceRemoved);

    if (!QDBusConnection::sessionBus().interface()->isServiceRegistered(SOLID_POWERMANAGEMENT_SERVICE))
    {
        qWarning() << SOLID_POWERMANAGEMENT_SERVICE << " is not registered";
        return;
    }    

    QDBusConnection::sessionBus().connect(QStringLiteral("org.freedesktop.PowerManagement"),
                                          QStringLiteral("/org/freedesktop/PowerManagement"),
                                          QStringLiteral("org.freedesktop.PowerManagement"),
                                          QStringLiteral("PowerSaveStatusChanged"),
                                          this,
                                          SLOT(onAcPlugStateChanged(bool)));

    if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                               QStringLiteral("/org/kde/Solid/PowerManagement"),
                                               SOLID_POWERMANAGEMENT_SERVICE,
                                               QStringLiteral("chargeStopThresholdChanged"),
                                               this,
                                               SLOT(onChargeStopThresholdChanged(int)))) {
        qDebug() << "error connecting to charge stop threshold changes via dbus";
    }

    const QList<Solid::Device> listBattery = Solid::Device::listFromType(Solid::DeviceInterface::Battery);

    if (listBattery.isEmpty())
    {
        this->setPrimaryBattery(nullptr);
        this->setHasBatteries(false);

        return;
    } //TODO remove?

    foreach (const Solid::Device &deviceBattery, listBattery)
    {
        auto battery = new Battery(deviceBattery);
        if(battery->isPowerSupply() && !m_primaryBattery)
        {
            m_primaryBattery = battery;
            emit this->primaryBatteryChanged();
        }

        m_batteries->addBattery(battery);
    }

    this->setPrimaryBattery(m_primaryBattery);
    this->setHasBatteries(m_batteries->count() > 0);


    QDBusMessage msg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                      QStringLiteral("/org/kde/Solid/PowerManagement"),
                                                      SOLID_POWERMANAGEMENT_SERVICE,
                                                      QStringLiteral("chargeStopThreshold"));
    QDBusPendingReply<int> reply = QDBusConnection::sessionBus().asyncCall(msg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
        QDBusPendingReply<int> reply = *watcher;
        if (!reply.isError()) {
            if(m_primaryBattery)
            {
                onChargeStopThresholdChanged(reply.value());
            }
        }
        watcher->deleteLater();
    });


    QDBusMessage acMsg = QDBusMessage::createMethodCall(QStringLiteral("org.freedesktop.PowerManagement"),
                                                      QStringLiteral("/org/freedesktop/PowerManagement"),
                                                      QStringLiteral("org.freedesktop.PowerManagement"),
                                                      QStringLiteral("GetPowerSaveStatus"));
    QDBusReply<bool> acReply = QDBusConnection::sessionBus().call(acMsg);
    onAcPlugStateChanged(acReply.isValid() ? acReply.value() : false);
}

void BatteryInfo::setPrimaryBattery(Battery *battery)
{
    if(battery == m_primaryBattery)
    {
        return;
    }

    m_primaryBattery = battery;
    emit this->primaryBatteryChanged();

}

void BatteryInfo::setHasBatteries(const bool &value)
{
    if(m_hasBatteries == value)
    {
        return;
    }

    m_hasBatteries = value;
    emit hasBatteriesChanged();
}

Battery::Battery(const Solid::Device &deviceBattery) : QObject(nullptr) //not parented. deleted by the BatteryModel
  ,m_battery(deviceBattery.as<Solid::Battery>())
  ,m_udi( deviceBattery.udi())
  ,m_vendor( deviceBattery.vendor())
  ,m_product( deviceBattery.product())
  ,m_type(batteryType(m_battery))
  ,m_state(m_battery->chargeState())
  ,m_stateName(batteryStateToString(m_state))
  ,m_remainingTime(m_battery->remainingTime())
  ,m_timeToEmpty(m_battery->timeToEmpty())
  ,m_timeToFull(m_battery->timeToFull())
{

    connect(m_battery, &Solid::Battery::timeToEmptyChanged, this, [this](qulonglong value, QString)
    {
        this->setTimeToEmpty(value);
    });

    connect(m_battery, &Solid::Battery::timeToFullChanged, this, [this](qulonglong value, QString)
    {
        this->setTimeToFull(value);
    });

    connect(m_battery, &Solid::Battery::chargeStateChanged, this, [this](int state, QString)
    {
        qDebug() << "Battery state changed" << state << batteryStateToString(state);
        this->setState(static_cast<Solid::Battery::ChargeState>(state));
    });

    connect(m_battery, &Solid::Battery::chargePercentChanged, this, [this](int percent, QString)
    {
        qDebug() << "Battery percent changed" << percent << m_battery->chargeState();

        emit this->percentChanged();
    });

    connect(m_battery, &Solid::Battery::energyChanged, this, [this](double, QString)
    {
        emit this->energyChanged();
    });

    connect(m_battery, &Solid::Battery::presentStateChanged, this, [this](bool present, QString)
    {
        qDebug() << "Battery present changed" << present;

        emit this->pluggedInChanged();
    });

    connect(m_battery, &Solid::Battery::remainingTimeChanged, this, [this](qulonglong time, QString)
    {
        qDebug() << "Battery remaining time changed" << time;
        this->setRemainingTime(time);
    });
}

Battery::~Battery()
{
    delete m_battery;
}

const QString &Battery::udi() const
{
    return m_udi;
}

QString Battery::batteryStateToString(int newState) const
{
    QString state(QStringLiteral("Unknown"));
    if (newState == Solid::Battery::NoCharge) {
        state = QLatin1String("NoCharge");
    } else if (newState == Solid::Battery::Charging) {
        state = QLatin1String("Charging");
    } else if (newState == Solid::Battery::Discharging) {
        state = QLatin1String("Discharging");
    } else if (newState == Solid::Battery::FullyCharged) {
        state = QLatin1String("FullyCharged");
    }

    return state;
}

void Battery::setState(const Solid::Battery::ChargeState &state)
{
    if(m_state == state)
    {
        return;
    }

    m_state = state;
    m_stateName = batteryStateToString(m_state);

    emit this->stateChanged();
}


QString Battery::batteryType(const Solid::Battery *battery) const
{
    switch (battery->type()) {
    case Solid::Battery::PrimaryBattery:
        return QStringLiteral("Battery");
        break;
    case Solid::Battery::UpsBattery:
        return QStringLiteral("Ups");
        break;
    case Solid::Battery::MonitorBattery:
        return QStringLiteral("Monitor");
        break;
    case Solid::Battery::MouseBattery:
        return QStringLiteral("Mouse");
        break;
    case Solid::Battery::KeyboardBattery:
        return QStringLiteral("Keyboard");
        break;
    case Solid::Battery::PdaBattery:
        return QStringLiteral("Pda");
        break;
    case Solid::Battery::PhoneBattery:
        return QStringLiteral("Phone");
        break;
    case Solid::Battery::GamingInputBattery:
        return QStringLiteral("GamingInput");
        break;
    case Solid::Battery::BluetoothBattery:
        return QStringLiteral("Bluetooth");
        break;
    default:
        return QStringLiteral("Unknown");
    }

    return QStringLiteral("Unknown");
}


const Solid::Battery::ChargeState &Battery::state() const
{
    return m_state;
}

const QString &Battery::vendor() const
{
    return m_vendor;
}

const QString &Battery::type() const
{
    return m_type;
}

const QString &Battery::product() const
{
    return m_product;
}

const QString &Battery::prettyName() const
{
    const QString batteryVendor = m_vendor;
    const QString batteryProduct = m_product;

    // Don't show battery name for primary power supply batteries. They usually have cryptic serial number names.
    const bool showBatteryName = m_type != QLatin1String("Battery")
            || !isPowerSupply();

    if (!batteryProduct.isEmpty() && batteryProduct != QLatin1String("Unknown Battery") && showBatteryName)
    {
        if (!batteryVendor.isEmpty()) {
            m_prettyName = QString(batteryVendor + ' ' + batteryProduct);
        } else {
            m_prettyName = batteryProduct;
        }
    } else
    {

        m_prettyName = "Battery";
    }

    return m_prettyName;

}

bool Battery::pluggedIn() const
{
    return m_battery->isPresent();
}

int Battery::percent() const
{
    return m_battery->chargePercent();
}

bool Battery::isPowerSupply() const
{
    return  m_battery->isPowerSupply();
}

double Battery::energy() const
{
    return m_battery->energy();
}

int Battery::capacity() const
{
    return m_battery->capacity();
}

void BatteryInfo::deviceRemoved(const QString &udi)
{
    if (m_batteries->contains(udi))
    {
        Solid::Device device(udi);
        Solid::Battery *battery = device.as<Solid::Battery>();
        if (battery)
            battery->disconnect();

        if(m_primaryBattery == m_batteries->battery(udi))
        {
            qWarning() << "Removing primary battery?";
        }

        m_batteries->removeBattery(udi);

        m_hasBatteries = m_batteries->count() > 0;
        emit hasBatteriesChanged();
    }
}

void BatteryInfo::deviceAdded(const QString &udi)
{
    Solid::Device device(udi);
    if (device.isValid())
    {
        const Solid::Battery *battery = device.as<Solid::Battery>();

        if (battery)
        {
            auto newBattery = new Battery(device);
            m_batteries->addBattery(newBattery);
        }
    }
}

void BatteryInfo::onChargeStopThresholdChanged(const int &value)
{
    m_primaryBattery->setChargeStopThreshold(value);
}

void BatteryInfo::onAcPlugStateChanged(const bool &value)
{
    if(value == m_acPluggedIn)
    {
        return;
    }

    m_acPluggedIn = value;
    emit acPluggedInChanged(m_acPluggedIn);
}


/**

void PowermanagementEngine::updateOverallBattery()
{
    const QList<Solid::Device> listBattery = Solid::Device::listFromType(Solid::DeviceInterface::Battery);
    bool hasCumulative = false;

    double energy = 0;
    double totalEnergy = 0;
    bool allFullyCharged = true;
    bool charging = false;
    bool noCharge = false;
    double totalPercentage = 0;
    int count = 0;

    foreach (const Solid::Device &deviceBattery, listBattery) {
        const Solid::Battery *battery = deviceBattery.as<Solid::Battery>();

        if (battery && battery->isPowerSupply()) {
            hasCumulative = true;

            energy += battery->energy();
            totalEnergy += battery->energyFull();
            totalPercentage += battery->chargePercent();
            allFullyCharged = allFullyCharged && (battery->chargeState() == Solid::Battery::FullyCharged);
            charging = charging || (battery->chargeState() == Solid::Battery::Charging);
            noCharge = noCharge || (battery->chargeState() == Solid::Battery::NoCharge);
            ++count;
        }
    }

    if (count == 1) {
        // Energy is sometimes way off causing us to show rubbish; this is a UPower issue
        // but anyway having just one battery and the tooltip showing strange readings
        // compared to the popup doesn't look polished.
        setData(QStringLiteral("Battery"), QStringLiteral("Percent"), totalPercentage);
    } else if (totalEnergy > 0) {
        setData(QStringLiteral("Battery"), QStringLiteral("Percent"), qRound(energy / totalEnergy * 100));
    } else if (count > 0) { // UPS don't have energy, see Bug 348588
        setData(QStringLiteral("Battery"), QStringLiteral("Percent"), qRound(totalPercentage / static_cast<qreal>(count)));
    } else {
        setData(QStringLiteral("Battery"), QStringLiteral("Percent"), 0);
    }

    if (hasCumulative) {
        if (allFullyCharged) {
            setData(QStringLiteral("Battery"), QStringLiteral("State"), "FullyCharged");
        } else if (charging) {
            setData(QStringLiteral("Battery"), QStringLiteral("State"), "Charging");
        } else if (noCharge) {
            setData(QStringLiteral("Battery"), QStringLiteral("State"), "NoCharge");
        } else {
            setData(QStringLiteral("Battery"), QStringLiteral("State"), "Discharging");
        }
    } else {
        setData(QStringLiteral("Battery"), QStringLiteral("State"), "Unknown");
    }

    setData(QStringLiteral("Battery"), QStringLiteral("Has Cumulative"), hasCumulative);
}
**/
qulonglong Battery::remainingTime() const
{
    return m_remainingTime;
}

int Battery::chargeStopThreshold() const
{
    return m_chargeStopThreshold;
}


void Battery::setRemainingTime(qulonglong newRemainingTime)
{
    if (m_remainingTime == newRemainingTime)
        return;

    m_remainingTime = newRemainingTime;
    emit remainingTimeChanged();
}


void Battery::setChargeStopThreshold(int newChargeStopThreshold)
{
    if (m_chargeStopThreshold == newChargeStopThreshold)
        return;
    m_chargeStopThreshold = newChargeStopThreshold;
    emit chargeStopThresholdChanged();
}

int BatteryModel::count() const
{
    return m_batteries.count();
}

const QString &Battery::stateName() const
{
    return m_stateName;
}

qulonglong Battery::timeToEmpty() const
{
    return m_timeToEmpty;
}

qulonglong Battery::timeToFull() const
{
    return m_timeToFull;
}

void Battery::setTimeToEmpty(qulonglong timeToEmpty)
{
    if (m_timeToEmpty == timeToEmpty)
        return;

    m_timeToEmpty = timeToEmpty;
    emit timeToEmptyChanged(m_timeToEmpty);
}

void Battery::setTimeToFull(qulonglong timeToFull)
{
    if (m_timeToFull == timeToFull)
        return;

    m_timeToFull = timeToFull;
    emit timeToFullChanged(m_timeToFull);
}
