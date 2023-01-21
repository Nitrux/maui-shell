import QtQuick 2.15
import QtQml 2.14
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.0
import org.maui.cask 1.0 as Cask

import org.mauikit.controls 1.2 as Maui
import QtQuick.Templates 2.15 as T

T.Container
{
    id: control
    implicitHeight: _cardsList.contentHeight + topPadding + bottomPadding

    clip: isMobile
    padding: isMobile ? 0 : _cardsList.spacing
    leftPadding: padding
    rightPadding: padding
    bottomPadding: padding
    topPadding: padding

    focus: true

    property int finalYPos : 0
    property bool opened : false
    property int alignment: Qt.AlignCenter

    property bool displayOverlay : win.formFactor !== Cask.MauiMan.Desktop

    signal overlayClicked()
    //        signal opened()
    signal closed()

    Keys.enabled: true
    Keys.onEscapePressed: control.overlayClicked()

    background: null
    data: MouseArea
    {
        id: _overlay
        visible: control.visible

        anchors.fill: parent

        parent: _cask.overlayTopPanel

        propagateComposedEvents: !control.displayOverlay
        preventStealing: control.displayOverlay

        onClicked:
        {
            control.overlayClicked()
            mouse.accepted = true
        }

        onPressed:
        {
            if(!control.displayOverlay)
            {
                control.overlayClicked()
                mouse.accepted = false
            }
        }

        Loader
        {
            active: Maui.Style.enableEffects && control.displayOverlay
            anchors.fill: parent

            sourceComponent: Item
            {
                FastBlur
                {
                    anchors.fill: parent
                    radius: 64
                    source: _cask.superOverlay

                    layer.enabled: true
                    layer.effect: Desaturate
                    {
                        desaturation: -1.2
                    }
                }
            }
        }

        Rectangle
        {
            visible: control.displayOverlay
            opacity: Math.min(0.7, (control.height/(availableGeometry.height* 0.7)))

            anchors.fill: parent
            color: Maui.Theme.backgroundColor

            Behavior on color
            {
                ColorAnimation
                {
                    easing.type: Easing.InQuad
                    duration: Maui.Style.units.shortDuration
                }
            }
        }

    }

    contentItem: ListView
    {
        id: _cardsList
        spacing: currentCard === -1  ? Maui.Style.space.medium : 0
        model: control.contentModel
        orientation: ListView.Vertical
        snapMode: ListView.NoSnap

        boundsBehavior: Flickable.StopAtBounds
        boundsMovement :Flickable.StopAtBounds

        //        interactive: Maui.Handy.isTouch
        highlightFollowsCurrentItem: true
        highlightMoveDuration: 0
        highlightResizeDuration : 0

        DragHandler
        {
            id: handler2
            dragThreshold: 100
            enabled: _cardsList.atYEnd
            //                enabled: popup.opened /*&& Maui.Handy.isTouch*/
            target: control
            yAxis.maximum: control.y
            //                yAxis.maximum: popup.finalYPos

            xAxis.enabled : false
            grabPermissions: PointerHandler.CanTakeOverFromItems | PointerHandler.CanTakeOverFromHandlersOfDifferentType | PointerHandler.ApprovesTakeOverByAnything
            onActiveChanged:
            {
                const condition = (handler2.centroid.scenePosition.y - handler2.centroid.scenePressPosition.y < -200)
                if(!active && condition)
                {
                    control.close()
                }else
                {
                    control.y = control.finalYPos
                }
            }
        }

        //        add: Transition
        //        {
        //            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 400 }
        ////            NumberAnimation { property: "scale"; from: 0; to: 1.0; duration: 400 }
        //        }

        displaced: Transition
        {
            NumberAnimation
            {
                properties: "x,y"; duration: 400; easing.type: Easing.OutBounce
            }
        }
    }

    Component.onDestruction:
    {
        console.log("DESTROY PANEL POPUP")
    }

    NumberAnimation
    {
        id: _slideDownAnimation
        target: control
        property: "y"
        duration: Maui.Style.units.shortDuration
        from: 0 - control.height
        to: control.finalYPos
        easing.type: Easing.OutInQuad
    }

    NumberAnimation
    {
        id: _slideUpAnimation
        target: control
        property: "y"
        duration: Maui.Style.units.shortDuration
        from: control.y
        to: 0 - control.height
        easing.type: Easing.InOutQuad
        onFinished:
        {
            control.clear()
            control.close()
        }
    }

    function close()
    {
        control.opened = false
        control.closed()
    }

    function open()
    {
        control.opened = true
        control.forceActiveFocus()
        //        control.opened()
    }

    function clear()
    {
        control.contentModel.clear()
    }

    function contains(card)
    {
        return control.contentModel.get(card.index) === card;
    }

    function addCard(card)
    {
        if(!isPanelCard(card))
        {
            return;
        }

        if(control.contains(card))
        {
            return;
        }

        control.addItem(card)
    }

    function slideDown()
    {
        popup.open()
        _slideDownAnimation.start()
    }

    function slideUp()
    {
        _slideUpAnimation.start()
    }
}

