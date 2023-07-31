import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls

import Qt5Compat.GraphicalEffects

import org.mauikit.controls as Maui
import org.maui.cask as Cask

import "items/calendar"
import "items/player"
import "items/sliders"
import "items/toggles"
import "items/notifications"
import "items/session"

Control
{
    id: control

    property rect availableGeometry
    readonly property bool floating : win.formFactor === Cask.MauiMan.Desktop
    readonly property bool cardsOverlapping : (_notificationsSection.popup.width + _statusSection.popup.width) > availableGeometry.width

    property bool hidden : contentItem.y === 0- control.height
    property bool autohide: false
    property bool labelsVisible: win.formFactor === Cask.MauiMan.Desktop

    Layout.fillWidth: true
    //    Layout.margins: floating ? Maui.Style.space.tiny : 0

    implicitHeight: implicitContentHeight + topPadding + bottomPadding

    spacing: padding
    padding: !floating ? 0 : Maui.Style.space.medium
    topPadding: padding
    bottomPadding: padding
    leftPadding: padding
    rightPadding: padding

    onCardsOverlappingChanged:
    {
        if(cardsOverlapping)
        {
            _notificationsSection.popup.close()
            _statusSection.popup.close()
        }
    }

    Behavior on padding
    {
        NumberAnimation
        {
            duration: Maui.Style.units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    Behavior on spacing
    {
        NumberAnimation
        {
            duration: Maui.Style.units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    background: Item
    {
        //        visible: !control.floating
        opacity:  _notificationsSection.popup.opened || _statusSection.popup.opened ? 1 : 0.8

        LinearGradient
        {
            anchors.fill: parent
            opacity: 0.8
            start: Qt.point(0, 0)
            end: Qt.point(0, height)
            gradient: Gradient
            {
                GradientStop { position: 0.0; color: "#333" }
                GradientStop { position: 0.7; color: "transparent" }
            }
        }

        Behavior on opacity
        {
            NumberAnimation
            {
                duration: Maui.Style.units.longDuration
                easing.type: Easing.InOutQuad
            }
        }

        Rectangle
        {
            anchors.fill: parent
            color: !control.floating ? Maui.Theme.backgroundColor : "transparent"

        }

    }

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

    data: Timer
    {
        id: _timer
        running: control.autohide && !control.hidden && !_handler.active && !(_notificationsSection.popup.opened || _statusSection.popup.opened) && !_statusSection.hovered
        interval: 4000
        //        triggeredOnStart: true
        onTriggered:
        {
            console.log("_timer triggered")
            if(control.autohide && !control.hidden && !_handler.active && !(_notificationsSection.popup.opened || _statusSection.popup.opened) && !_statusSection.hovered)
            {
                control.hide()
            }
        }
    }

    contentItem: RowLayout
    {
        id: _layout
        spacing: control.spacing

        Cask.PanelSection
        {
            id: _notificationsSection
            alignment: Qt.AlignLeft
            availableGeometry: control.availableGeometry
            spacing: control.spacing
            Layout.fillWidth: true

            Connections
            {
                target: _notificationsSection.popup
                function onOpenedChanged() //if the cards are open and the avaliable width is not enough close the right section
                {
                    if(target.opened && _statusSection.popup.opened && control.cardsOverlapping)
                    {
                        _statusSection.popup.close()
                    }
                }
            }

            NotificationsItem
            {
            }

            CalendarItem
            {
            }
        }

        Cask.PanelSection
        {
            id: _statusSection
            alignment: Qt.AlignRight
            availableGeometry : control.availableGeometry
            spacing: control.spacing

            Connections
            {
                target: _statusSection.popup
                function onOpenedChanged()
                {
                    if(target.opened && _notificationsSection.popup.opened && control.cardsOverlapping)
                    {
                        _notificationsSection.popup.close()
                    }
                }
            }

            //            Cask.PanelItem
            //            {
            //                id: _revealer
            ////                visible: _statusSection.visibleChildren > 0
            //                icon.name: checked ? "arrow-right" : "arrow-left"
            //                checkable: true
            //                width: height
            //                leftPadding: 0
            //                rightPadding: 0

            //                data: Timer
            //                {
            //                    running: _revealer.checked && !_statusSection.popup.opened && !_statusSection.hovered
            //                    interval: 4000
            //                    onTriggered: _revealer.toggle()
            //                }
            //            }

            TogglesItem
            {
                id: _togglesItem
            }

            SlidersItem
            {
                id: _slidersItem
            }

            AudioPlayerItem
            {
                id: _mediaController
                visible: /*_revealer.checked || */isPlaying
            }

            SessionItem
            {
                id: _sessionItem
                visible: !isMobile
            }
        }
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
}

