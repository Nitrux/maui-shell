import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask

import "items/calendar"
import "items/player"
import "items/sliders"
import "items/toggles"
import "items/notifications"
import "items/session"

Control
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

    contentItem: RowLayout
    {
        id: _layout

        Cask.PanelSection
        {
            id: _notificationsSection

            Layout.fillWidth: true
//            Layout.fillHeight: true

            position : ToolBar.Header
            popWidth: 320
            alignment: Qt.AlignLeft
//            background: Rectangle
//            {
//                color: "red"
//            }


            NotificationsItem
            {
                onClicked: _notificationsSection.open(card.index)
//                anchors.verticalCenter: parent.verticalCenter
            }

            CalendarItem
            {
                onClicked: _notificationsSection.open(card.index)
//                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Cask.PanelSection
        {
            id: _statusSection
            Layout.alignment: Qt.AlignRight
//            Layout.fillHeight: true

            position : ToolBar.Header
            alignment: Qt.AlignRight

            popWidth: 320
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
                    running: _revealer.checked
                    interval: 4000
                    onTriggered: _revealer.toggle()
                }

            }

            TogglesItem
            {
                onClicked: _statusSection.open(card.index)
//                anchors.verticalCenter: parent.verticalCenter

                //                visible: !isMobile
            }

            SlidersItem
            {
                visible: _revealer.checked
                onClicked: _statusSection.open(card.index)
//                anchors.verticalCenter: parent.verticalCenter

//                                visible: !isMobile
            }

            AudioPlayerItem
            {
                visible: _revealer.checked
//                                visible: !isMobile
                onClicked: _statusSection.open(card.index)
//                anchors.verticalCenter: parent.verticalCenter

            }

            SessionItem
            {
                visible: _revealer.checked
                onClicked: _statusSection.open(card.index)
                anchors.verticalCenter: parent.verticalCenter


            }
        }
    }
}

