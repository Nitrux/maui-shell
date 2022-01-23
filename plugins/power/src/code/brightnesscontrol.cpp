#include "brightnesscontrol.h"
#include "powermanagementengine.h"
#include <QDebug>

BrightnessControl::BrightnessControl(QObject *parent) : QObject(parent)
  ,m_engine(new PowermanagementEngine(this))
  ,m_screenBrightness(1400)
  ,m_keyboardBrightness(-1)
  ,m_maximumScreenBrightness(100)
  ,m_maximumKeyboardBrightness(-1)
{
    connect(m_engine, &PowermanagementEngine::maximumScreenBrightnessChanged, this, [this](int value)
    {
        if(m_maximumScreenBrightness == value)
        {
            return;
        }

        qDebug() <<"CONTROL" << "Maximum Screen brigthness is" << value;
        m_maximumScreenBrightness = value;
        emit this->maximumScreenBrightnessChanged(m_maximumKeyboardBrightness);
//        emit this->screenBrightnessChanged(m_screenBrightness);
    });

    connect(m_engine, &PowermanagementEngine::screenBrightnessChanged, this, &BrightnessControl::setScreenBrightness);

    connect(m_engine, &PowermanagementEngine::screenBrightnessAvailableChanged, this, &BrightnessControl::setScreenBrightnessAvailable);

    m_engine->sourceRequestEvent("PowerDevil");
}

int BrightnessControl::screenBrightness() const
{
    return m_screenBrightness;
}

int BrightnessControl::keyboardBrightness() const
{
    return m_keyboardBrightness;
}

int BrightnessControl::maximumScreenBrightness() const
{
    return m_maximumScreenBrightness;
}

int BrightnessControl::maximumKeyboardBrightness() const
{
    return m_maximumKeyboardBrightness;
}

void BrightnessControl::setScreenBrightness(int screenBrightness)
{
    m_screenBrightness = screenBrightness;
    emit screenBrightnessChanged(m_screenBrightness);
}

void BrightnessControl::setKeyboardBrightness(int keyboardBrightness)
{
    if (m_keyboardBrightness == keyboardBrightness)
        return;

    m_keyboardBrightness = keyboardBrightness;
    emit keyboardBrightnessChanged(m_keyboardBrightness);
}

void BrightnessControl::changeScreenBrightness(int value, bool silent)
{
    m_engine->setScreenBrightness(value, silent);
}

bool BrightnessControl::screenBrightnessAvailable() const
{
    return m_screenBrightnessAvailable;
}

void BrightnessControl::setScreenBrightnessAvailable(bool newScreenBrightnessAvailable)
{
    if (m_screenBrightnessAvailable == newScreenBrightnessAvailable)
        return;
    m_screenBrightnessAvailable = newScreenBrightnessAvailable;
    emit screenBrightnessAvailableChanged();
}

bool BrightnessControl::keyboardBrightnessAvailable() const
{
    return m_keyboardBrightnessAvailable;
}

void BrightnessControl::setKeyboardBrightnessAvailable(bool newKeyboardBrightnessAvailable)
{
    if (m_keyboardBrightnessAvailable == newKeyboardBrightnessAvailable)
        return;
    m_keyboardBrightnessAvailable = newKeyboardBrightnessAvailable;
    emit keyboardBrightnessAvailableChanged();
}
