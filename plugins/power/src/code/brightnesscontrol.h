#pragma once
#include <QObject>

class BrightnessControl : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int screenBrightness READ screenBrightness NOTIFY screenBrightnessChanged FINAL)
    Q_PROPERTY(int keyboardBrightness READ keyboardBrightness NOTIFY keyboardBrightnessChanged FINAL)

    Q_PROPERTY(bool screenBrightnessAvailable READ screenBrightnessAvailable NOTIFY screenBrightnessAvailableChanged FINAL)
    Q_PROPERTY(bool keyboardBrightnessAvailable READ keyboardBrightnessAvailable NOTIFY keyboardBrightnessAvailableChanged FINAL)

    Q_PROPERTY(int maximumScreenBrightness READ maximumScreenBrightness NOTIFY maximumScreenBrightnessChanged FINAL)
    Q_PROPERTY(int maximumKeyboardBrightness READ maximumKeyboardBrightness NOTIFY maximumKeyboardBrightnessChanged FINAL)

    Q_PROPERTY(bool lidPresent READ lidPresent NOTIFY lidPresentChanged FINAL)
    Q_PROPERTY(bool triggersLidAction READ triggersLidAction NOTIFY triggersLidActionChanged FINAL)

public:
    explicit BrightnessControl(QObject *parent = nullptr);

    int screenBrightness() const;
    void setScreenBrightness(int screenBrightness);

    int keyboardBrightness() const;
    void setKeyboardBrightness(int keyboardBrightness);

    int maximumScreenBrightness() const;
    void setMaximumScreenBrightness(int value);

    int maximumKeyboardBrightness() const;
    void setMaximumKeyboardBrightness(int value);

    bool screenBrightnessAvailable() const;
    void setScreenBrightnessAvailable(bool newScreenBrightnessAvailable);

    bool keyboardBrightnessAvailable() const;
    void setKeyboardBrightnessAvailable(bool newKeyboardBrightnessAvailable);

    bool lidPresent() const;
    void setLidPresent(bool lidPresent);

    bool triggersLidAction() const;
    void setTriggersLidAction(bool triggersLidAction);

public slots:
    void changeScreenBrightness( int value, bool silent);
    void changeKeyboardBrightness( int value, bool silent);

signals:
    void screenBrightnessChanged(int screenBrightness);
    void keyboardBrightnessChanged(int keyboardBrightness);
    void maximumScreenBrightnessChanged(int maximumScreenBrightness);
    void maximumKeyboardBrightnessChanged(int maximumKeyboardBrightness);

    void screenBrightnessAvailableChanged();
    void keyboardBrightnessAvailableChanged();
    void lidPresentChanged(bool lidPresent);
    void triggersLidActionChanged(bool triggersLidAction);

private:
    int m_screenBrightness;
    int m_keyboardBrightness;
    int m_maximumScreenBrightness;
    int m_maximumKeyboardBrightness;

    bool m_screenBrightnessAvailable = false;
    bool m_keyboardBrightnessAvailable = false;
    bool m_lidPresent = false;
    bool m_triggersLidAction = false;
};

