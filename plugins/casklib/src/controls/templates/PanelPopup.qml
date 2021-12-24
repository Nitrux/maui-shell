import QtQuick 2.13
import QtQml 2.14
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13
import QtGraphicalEffects 1.0
import org.maui.cask 1.0 as Cask

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

Item
{
    id: control
    //    visible: y > 0
    focus: true

    property bool opened : false
    property alias container : _cards
    property alias count: _cards.count
    implicitHeight: _cardsList.contentHeight + (_cards.padding*2)
    property alias contentChildren : _cards.contentChildren
    property alias padding : _cards.padding

    signal overlayClicked()
//    signal opened()
    signal closed()

    Keys.enabled: true
    Keys.onEscapePressed: control.overlayClicked()
    Keys.onSpacePressed: control.overlayClicked()

    Item
    {
        id:_overlay
        opacity:  control.opacity

        visible: control.visible
        anchors.fill: parent
        parent: _cask.overlayTopPanel;


        Item
        {
            visible: win.formFactor !== Cask.Env.Desktop
            anchors.fill: parent
            Image
            {
                id: _img
                anchors.fill: parent
                fillMode: Image.PreserveAspectCrop
                source: _cask.backgroundImage
            }

            FastBlur
            {
                anchors.fill: parent
                id: fastBlur
                source: _img
                radius: 64
                transparentBorder: false
                cached: true
            }

            Rectangle
            {
                opacity: Math.min(win.formFactor !== Cask.Env.Desktop ? 0.95 : 0.7, control.opacity)

                anchors.fill: parent
                Kirigami.Theme.inherit: false
                Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
                color: Kirigami.Theme.backgroundColor
                radius: Maui.Style.radiusV
                //            border.color: Qt.tint(Kirigami.Theme.textColor, Qt.rgba(Kirigami.Theme.backgroundColor.r, Kirigami.Theme.backgroundColor.g, Kirigami.Theme.backgroundColor.b, 0.7))
                Behavior on opacity
                {
                    NumberAnimation
                    {
                        duration: Kirigami.Units.longDuration
                        easing.type: Easing.OutInQuad
                    }
                }
            }
        }

        MouseArea
        {
            anchors.fill: parent
            propagateComposedEvents: win.formFactor === Cask.Env.Desktop
            preventStealing: win.formFactor !== Cask.Env.Desktop
            onPressed:
            {
                control.overlayClicked()
                mouse.accepted = win.formFactor !== Cask.Env.Desktop
            }
        }
    }

    Container
    {
        id: _cards
        anchors.fill: parent
        clip: false
        padding: isMobile ? 0 : Maui.Style.space.medium
        leftPadding: padding
        rightPadding: padding
        bottomPadding: padding
        topPadding: padding

        contentItem: ListView
        {
            id: _cardsList
            spacing: currentCard === -1  ? Maui.Style.space.medium : 0
            model: _cards.contentModel
            orientation: ListView.Vertical
            snapMode: ListView.NoSnap

            boundsBehavior: Flickable.StopAtBounds
            boundsMovement :Flickable.StopAtBounds

            //            interactive: Kirigami.Settings.hasTransientTouchInput /*&& (control.selectionMode ? _listView.position.x > 84 : true)*/
            highlightFollowsCurrentItem: true
            highlightMoveDuration: 0
            highlightResizeDuration : 0
            //            pressDelay: 1000
        }
    }


    Component.onDestruction:
    {
        console.log("DESTROY PANEL POPUP")
    }

}
