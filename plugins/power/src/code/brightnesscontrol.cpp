#include "brightnesscontrol.h"
#include "powermanagementengine.h"
#include <QDebug>

BrightnessControl::BrightnessControl(QObject *parent) : QObject(parent)
  ,m_engine(new PowermanagementEngine(this))
  ,m_maximumScreenBrightness(100)
  ,m_maximumKeyboardBrightness(100)
  ,m_screenBrightness(50)
  ,m_keyboardBrightness(50)
{
    m_engine->sourceRequestEvent("PowerDevil");
    connect(m_engine, &PowermanagementEngine::screenBrightnessChanged, [this](int value)
    {
        qDebug() << "SCreen brigthness is" << value;
    });
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
    if (m_screenBrightness == screenBrightness)
        return;

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
