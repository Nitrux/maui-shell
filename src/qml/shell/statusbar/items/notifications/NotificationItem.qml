import QtQuick 2.15
import QtQuick.Controls 2.15

import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.3 as Maui
import QtGraphicalEffects 1.0

import org.maui.cask 1.0 as Cask

ItemDelegate
{
    id: control
    implicitHeight: 80
    opacity: (width-Math.abs(x))/width
    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
    property bool autoDismiss : false

    property alias title : control.text
    property string message
    property alias iconSource : _template.iconSource
    property alias imageSource : _template.imageSource


    signal dismissed()

    Maui.CloseButton
    {
        anchors.right: parent.right
        anchors.top: parent.top
        onClicked: control.dismissed()
    }

    contentItem: Maui.ListItemTemplate
    {
        id: _template
        iconSource: "documentinfo"
        label1.text: control.title
        label2.text: control.message
        label2.wrapMode: Text.Wrap
        iconSizeHint: Maui.Style.iconSizes.medium
        spacing: Maui.Style.space.medium
        headerSizeHint: iconSizeHint + Maui.Style.space.big
    }


    background: Item
    {
        Rectangle
        {
            id: _backRec
            anchors.fill: parent
            color:  Kirigami.Theme.backgroundColor
            radius: 12
        }

        DropShadow
        {
            transparentBorder: true
            anchors.fill: _backRec
            horizontalOffset: 0
            verticalOffset: 0
            radius: 8.0
            samples: 17
            color: Qt.rgba(0,0,0,0.2)
            source: _backRec
        }
    }

    onClicked: console.log("NOF", control.x)

    DragHandler
    {
        id: _handler
        target: control
        xAxis.enabled: true
        xAxis.maximum: 0

        yAxis.enabled: false

        grabPermissions: TapHandler.CanTakeOverFromAnything
        onActiveChanged:
        {
            if(!_handler.active)
            {
                if(control.x < -100)
                {
                    control.dismissed()
                }else
                {
                    control.x = 0
                }
            }
        }
    }

    Timer
    {
        id: _timer

        interval: 5000
        running: !control.hovered && autoDismiss
        onTriggered: destroyAnimationImpl.start()

        //        triggeredOnStart: true
    }


    SequentialAnimation
    {
        id: destroyAnimationImpl
        ParallelAnimation
        {
            NumberAnimation
            {
                target: control;
                property: "x";
                easing.type: Easing.OutInQuad
                to: 0-control.width;
                duration: Kirigami.Units.longDuration

            }
        }
        ScriptAction { script: { control.dismissed() } }
    }


}
