import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.mauikit.controls as Maui

import QtQuick.Effects

import org.maui.cask as Cask

ItemDelegate
{
    id: control
    implicitHeight: _layout.implicitHeight + topPadding + bottomPadding
    opacity: width > 0 ? (width - Math.abs(x)) / width : 0

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0
    property bool autoDismiss : false

    property alias title : control.text
    property string message
    property alias iconSource : _template.iconSource
    property alias imageSource : _template.imageSource
    property alias timeout : _timer.interval
    property int urgency : -1
    property int notificationId : 0
    property bool hasDefaultAction : false
    property var actions : []

    signal dismissed()
    signal expired()

    Maui.CloseButton
    {
        visible: control.hovered
        anchors.right: parent.right
        anchors.top: parent.top
        onClicked: control.dismissed()
    }

    contentItem: ColumnLayout
    {
        id: _layout
        spacing: 0

        Maui.ListItemTemplate
        {
            id: _template
            Layout.fillWidth: true
            Layout.margins: Maui.Style.space.medium
            iconSource: "documentinfo"
            label1.text: control.title
            label2.text: control.message
            label2.wrapMode: Text.Wrap
            iconSizeHint: Maui.Style.iconSizes.big
            spacing: Maui.Style.space.medium
            headerSizeHint: iconSizeHint + Maui.Style.space.big
        }

        Rectangle
        {
            Layout.fillWidth: true
            Layout.margins: Maui.Style.space.medium
            visible: control.urgency > -1
            implicitHeight: 4
            radius: 2
            color: switch(control.urgency)
                   {
                   case 0: return Maui.Theme.positiveBackgroundColor;
                   case 1: return Maui.Theme.neutralBackgroundColor;
                   case 2: return Maui.Theme.negativeBackgroundColor
                   default: return Maui.Theme.backgroundColor
                   }
        }

    }

    // Maui Core's notification server only exposes the default action to QML
    // (NotificationsModel.invokeDefaultAction), so clicking the notification
    // triggers that one.
    onClicked:
    {
        if(control.hasDefaultAction && CaskShell.notifications)
            CaskShell.notifications.notificationsModel.invokeDefaultAction(control.notificationId)
    }

    background: Item
    {
        Rectangle
        {
            id: _backRec
            anchors.fill: parent
            color:  Maui.Theme.backgroundColor
            radius: 10

            Behavior on color
            {
                ColorAnimation
                {
                    easing.type: Easing.InQuad
                    duration: Maui.Style.units.shortDuration
                }
            }

            layer.enabled: true
            layer.effect: MultiEffect
            {
                autoPaddingEnabled: true
                shadowEnabled: true
                shadowColor: "#000000"
            }
        }
    }


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
                duration: Maui.Style.units.longDuration

            }
        }
        ScriptAction { script: { control.expired() } }
    }
}
