#pragma once
#include <QObject>
class PowermanagementEngine;
class BrightnessControl : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int screenBrightness READ screenBrightness NOTIFY screenBrightnessChanged CONSTANT)
    Q_PROPERTY(int keyboardBrightness READ keyboardBrightness NOTIFY keyboardBrightnessChanged CONSTANT)

    Q_PROPERTY(bool screenBrightnessAvailable READ screenBrightnessAvailable NOTIFY screenBrightnessAvailableChanged CONSTANT)
    Q_PROPERTY(bool keyboardBrightnessAvailable READ keyboardBrightnessAvailable NOTIFY keyboardBrightnessAvailableChanged CONSTANT)

    Q_PROPERTY(int maximumScreenBrightness READ maximumScreenBrightness NOTIFY maximumScreenBrightnessChanged CONSTANT)
    Q_PROPERTY(int maximumKeyboardBrightness READ maximumKeyboardBrightness NOTIFY maximumKeyboardBrightnessChanged CONSTANT)

public:
    explicit BrightnessControl(QObject *parent = nullptr);

    int screenBrightness() const;

    int keyboardBrightness() const;

    int maximumScreenBrightness() const;

    int maximumKeyboardBrightness() const;

    bool screenBrightnessAvailable() const;
    void setScreenBrightnessAvailable(bool newScreenBrightnessAvailable);

    bool keyboardBrightnessAvailable() const;
    void setKeyboardBrightnessAvailable(bool newKeyboardBrightnessAvailable);
    void setScreenBrightness(int screenBrightness);
    void setKeyboardBrightness(int keyboardBrightness);

public slots:
    void changeScreenBrightness( int value, bool silent);

signals:
    void screenBrightnessChanged(int screenBrightness);
    void keyboardBrightnessChanged(int keyboardBrightness);
    void maximumScreenBrightnessChanged(int maximumScreenBrightness);
    void maximumKeyboardBrightnessChanged(int maximumKeyboardBrightness);

    void screenBrightnessAvailableChanged();

    void keyboardBrightnessAvailableChanged();

private:
    PowermanagementEngine *m_engine;
    int m_screenBrightness;

    int m_keyboardBrightness;

    int m_maximumScreenBrightness;

    int m_maximumKeyboardBrightness;
    bool m_screenBrightnessAvailable = false;
    bool m_keyboardBrightnessAvailable = false;
};

