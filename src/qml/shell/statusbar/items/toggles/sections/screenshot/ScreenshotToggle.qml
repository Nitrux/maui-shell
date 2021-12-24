import QtQuick 2.15
import QtQuick.Controls 2.15

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask
import "../.."

ToggleTemplate
{
    id: control
    text: i18n("Screenshot")
    icon.name:  "edit-image-face-detect"
    page: ScreenshotPage
    {

    }

    Rectangle
    {
        id: _rec
        visible: control.visible
        parent: _cask.overlayTopPanel
        color: "orange"
        anchors.fill: parent
        opacity: 0
        PropertyAnimation
        {
            id: animation;
            target: _rec;
            property: "opacity";
            to: 90;
            duration: 500

            onFinished:
            {
                _rec.opacity = 0
                _statusSection.close()

                _cask.grabToImage(function(result) {
                    result.saveToFile("something-%1.png".arg(new Date().toLocaleTimeString()));
                });
            }
        }


    }

}
