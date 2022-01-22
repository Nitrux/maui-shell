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
        qDebug() << "MAximum SCreen brigthness is" << value;
        m_maximumScreenBrightness = value;
        emit this->maximumScreenBrightnessChanged(m_maximumKeyboardBrightness);
        emit this->screenBrightnessChanged(m_screenBrightness);
    });

    connect(m_engine, &PowermanagementEngine::screenBrightnessChanged, this, [this](int value)
    {
        qDebug() << "SCreen brigthness is" << value;
        this->setScreenBrightness(value);
    });

//    connect(m_engine, &PowermanagementEngine::screenBrightnessAvailableChanged, this, [this](bool value)
//    {
//        qDebug() << "SCreen brigthness avaliable" << value;
//        this->setScreenBrightnessAvailable(value);
//    });
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
