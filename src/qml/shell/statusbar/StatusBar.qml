import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui
import QtQuick.Templates 2.15 as T

import org.maui.cask 1.0 as Cask

import "items/calendar"
import "items/player"
import "items/sliders"
import "items/toggles"
import "items/notifications"
import "items/session"

T.Control
{
    id: control

    readonly property bool floating : win.formFactor === Cask.Env.Desktop
    Layout.fillWidth: true
    Layout.margins: floating ? Maui.Style.space.tiny : 0

    implicitHeight:  implicitContentHeight + topPadding + bottomPadding

    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary

    padding: Maui.Style.space.tiny
    topPadding: padding
    bottomPadding: padding
    leftPadding: padding
    rightPadding: padding

    Behavior on  Layout.margins
    {
        NumberAnimation
        {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    //    Label
    //    {
    //        text: control.height + " / " +  _notificationsSection.height
    //        anchors.centerIn: parent
    //    }

    background: Item
    {
        visible: !control.floating
        opacity: 0.8
        Rectangle
        {
            id: _rec
            anchors.fill: parent
            //            radius: control.floating ? 6 : 0
            color: Kirigami.Theme.backgroundColor
        }

        //        DropShadow
        //        {
        //            visible: control.floating
        //            transparentBorder: true
        //            anchors.fill: parent
        //            horizontalOffset: 0
        //            verticalOffset: 0
        //            radius: 8.0
        //            samples: 17
        //            color: Qt.rgba(0,0,0,0.5)
        //            source: _rec
        //        }
    }
    function show()
    {
        control.contentItem.y = control.topPadding
        if(autohide)
        {
            _timer.restart()
        }
    }

    function hide()
    {
        control.contentItem.y = 0- control.height
    }
    property bool hidden : contentItem.y === 0- control.height
    property bool autohide: false

    Rectangle
    {
        anchors.fill: parent
        color: "pink"
        opacity: 0.4
        visible: control.autohide

        DragHandler
        {
            id: _handler
            enabled: control.hidden && control.autohide
            target: control.contentItem
            xAxis.enabled: false
            yAxis.maximum: control.topPadding
//            grabPermissions: PointerHandler.CanTakeOverFromAnything
            onActiveChanged:
            {
                if(!active)
                {
                    control.show()
                }
            }
        }
    }


    Timer
    {
        id: _timer
        running: control.autohide && !control.hidden && !_handler.active && !(_notificationsSection.popup.opened || _statusSection.popup.opened)
        interval: 4000
//        triggeredOnStart: true
        onTriggered:
        {
            console.log("_timer triggered")
            if(control.autohide && !control.hidden && !_handler.active && !(_notificationsSection.popup.opened || _statusSection.popup.opened))
            {
                control.hide()
            }
        }
    }

//    Label
//    {
//        text: control.contentItem.y + " - " + control.hidden
//    }

    contentItem: RowLayout
    {
        id: _layout


        Cask.PanelSection
        {
            id: _notificationsSection

            Layout.fillWidth: true
            //            Layout.fillHeight: true
            availableGeometry : desktop.availableGeometry

            popWidth: 320
            alignment: Qt.AlignLeft
            //            background: Rectangle
            //            {
            //                color: "red"
            //            }


            NotificationsItem
            {
//                onClicked: _notificationsSection.open(card.index)
                //                anchors.verticalCenter: parent.verticalCenter
            }

            CalendarItem
            {
//                onClicked: _notificationsSection.open(card.index)
                //                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Cask.PanelSection
        {
            id: _statusSection
            Layout.alignment: Qt.AlignRight
            //            Layout.fillHeight: true

            alignment: Qt.AlignRight
            availableGeometry : desktop.availableGeometry

            popWidth: 340
            //            background: Rectangle
            //            {
            //                color: "red"
            //            }


            Cask.PanelItem
            {
                id: _revealer
                icon.name: checked ? "arrow-right" : "arrow-left"
                checkable: true
                width: height
                leftPadding: 0
                rightPadding: 0

                Timer
                {
                    running: _revealer.checked && !_statusSection.popup.opened
                    interval: 4000
                    onTriggered: _revealer.toggle()
                }

            }

            TogglesItem
            {
//                onClicked: _statusSection.open(card.index)
                //                anchors.verticalCenter: parent.verticalCenter

                //                visible: !isMobile
            }

            SlidersItem
            {
                visible: _revealer.checked
//                onClicked: _statusSection.open(card.index)
                //                anchors.verticalCenter: parent.verticalCenter

                //                                visible: !isMobile
            }

            AudioPlayerItem
            {
                visible: _revealer.checked
//                onClicked: _statusSection.open(card.index)
            }

            SessionItem
            {
//                onClicked: card.visible ? _statusSection.close() : _statusSection.open(card.index)
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}

