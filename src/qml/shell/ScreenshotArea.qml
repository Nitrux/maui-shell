import QtQuick 2.15
import QtQuick.Controls 2.15
import QtMultimedia 5.15

import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask

Item
{

    id: control

    property bool running : false

    Rectangle
    {
        id: _color
        visible: _animation.running
        anchors.fill: parent
        color: Maui.Theme.textColor
        opacity: 0.4

        ParallelAnimation
        {
            id: _animation

            OpacityAnimator
            {
                target: _color
                from: 0.4
                to:0
            }
            SequentialAnimation
            {

                SequentialAnimation
                {
                    ScaleAnimator
                    {
                        target: _color
                        from: 1
                        to: 0.95
                        duration: 50

                        easing.type :Easing.OutQuad
                    }

                    ScaleAnimator
                    {
                        target: _color
                        from: 0.95
                        to: 1
                        duration: 50
                        easing.type :Easing.InOutQuad
                    }
                }

            }

            onFinished:
            {
                _color.opacity = 0.4
                control.running = false
            }
        }
    }

    SoundEffect
    {
        id: playSound
        source: "qrc:/sounds/camera-shutter.wav"
    }

    Connections
    {
        target: Cask.Server.screenshot
        function onTakeScreenshot(type, id)
        {
            console.log("GRAB SCREEN FROM THE SERVER REQUEDTED", id, type)
            switch(type)
            {
            case Cask.CaskScreenshot.CurrentScreen: control.grabCurrentScreen(id); return;
            case Cask.CaskScreenshot.AllScreens: control.grabAllScreens(id); return;
            case Cask.CaskScreenshot.CurrentWindow: control.grabCurrentWindow(id); return;
            }
        }
    }

    function animate()
    {
        _animation.start()
        playSound.play()
    }

    function grabCurrentScreen(id : string)
    {
        control.running = true
        _cask.grabToImage(function(result) {
            let where = Cask.Server.screenshot.saveDir +String("/screenshot-%1.png").arg(new Date().toLocaleTimeString())
            result.saveToFile(where);
            if(ok)
            {
                        Cask.Server.screenshot.setScreenshotReady(where, id)
            }

            console.log("SCREENSHOT REULT ", result.url, where)

        });
        control.animate()
    }

    function grabAllScreens(id: string)
    {

    }

    function grabCurrentWindow(id : string)
    {
        const window = _zpaces.allSurfaces.activeWindow

        if(window == null)
        {
            console.log("No window to grab screenshot")
        }
        console.log(window.id, window.appName)

        control.running = true
        const chrome = window.chrome;

        console.log("Chrome:" , chrome)

        chrome.grabToImage(function(result) {
            let where = Cask.Server.screenshot.saveDir +String("/screenshot-%1.png").arg(new Date().toLocaleTimeString())

            result.saveToFile(where);
        });
        control.animate()

    }

    function grabCustomArea(rect, id: string)
    {

    }
}
