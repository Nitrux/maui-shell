#pragma once
#include <QObject>
class PowermanagementEngine;
class BrightnessControl : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int screenBrightness READ screenBrightness WRITE setScreenBrightness NOTIFY screenBrightnessChanged)
    Q_PROPERTY(int keyboardBrightness READ keyboardBrightness WRITE setKeyboardBrightness NOTIFY keyboardBrightnessChanged)
    Q_PROPERTY(int maximumScreenBrightness READ maximumScreenBrightness NOTIFY maximumScreenBrightnessChanged CONSTANT)
    Q_PROPERTY(int maximumKeyboardBrightness READ maximumKeyboardBrightness NOTIFY maximumKeyboardBrightnessChanged CONSTANT)

public:
    explicit BrightnessControl(QObject *parent = nullptr);

    int screenBrightness() const;

    int keyboardBrightness() const;

    int maximumScreenBrightness() const;

    int maximumKeyboardBrightness() const;

public slots:
    void setScreenBrightness(int screenBrightness);

    void setKeyboardBrightness(int keyboardBrightness);

signals:
    void screenBrightnessChanged(int screenBrightness);
    void keyboardBrightnessChanged(int keyboardBrightness);
    void maximumScreenBrightnessChanged(int maximumScreenBrightness);
    void maximumKeyboardBrightnessChanged(int maximumKeyboardBrightness);

private:
    PowermanagementEngine *m_engine;
    int m_screenBrightness;

    int m_keyboardBrightness;

    int m_maximumScreenBrightness;

    int m_maximumKeyboardBrightness;
};

