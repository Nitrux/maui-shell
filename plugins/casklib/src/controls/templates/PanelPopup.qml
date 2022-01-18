import QtQuick 2.15
import QtQml 2.14
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.0
import org.maui.cask 1.0 as Cask

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui
import QtQuick.Templates 2.15 as T

T.Container
{
    id: control
    implicitHeight: _cardsList.contentHeight + topPadding + bottomPadding

    clip: isMobile
    padding: 0
    leftPadding: padding
    rightPadding: padding
    bottomPadding: padding
    topPadding: padding

    focus: true

    property int finalYPos : 0
    property bool opened : false
    property int alignment: Qt.AlignCenter

    signal overlayClicked()
//        signal opened()
    signal closed()

    Keys.enabled: true
    Keys.onEscapePressed: control.overlayClicked()

    background: null
    data: Item
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
//            Image
//            {
//                id: _img
//                anchors.fill: parent
//                fillMode: Image.PreserveAspectCrop
//                source: _swipeView.currentItem.backgroundImage
//            }

//            FastBlur
//            {
//                anchors.fill: parent
//                id: fastBlur
//                source: _img
//                radius: 64
//                transparentBorder: false
//                cached: true
//            }

            Rectangle
            {
                opacity: (control.height/availableGeometry.height)

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
    //                enabled: popup.opened /*&& Maui.Handy.isTouch*/
            target: control
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
        duration: Kirigami.Units.shortDuration
        from: 0 - control.height
        to: control.finalYPos
        easing.type: Easing.OutInQuad
    }

    NumberAnimation
    {
        id: _slideUpAnimation
        target: control
        property: "y"
        duration: Kirigami.Units.shortDuration
        from: control.y
        to: 0 - control.height
        easing.type: Easing.InOutQuad
        onFinished: {
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

