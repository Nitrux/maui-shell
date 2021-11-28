import QtQuick 2.13
import QtQml 2.14
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13
import QtGraphicalEffects 1.0

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

Item
{
    id: control
    //    visible: y > 0

    property bool opened : false
    property alias container : _cards
    property alias count: _cards.count
    property alias implicitHeight: _cardsList.contentHeight
    //    property alias implicitWidth: _cardsList.contentHeight
    property alias contentChildren : _cards.contentChildren

    property int position

    signal overlayClicked()

    Item
    {
        id:_overlay
        opacity:  control.opacity

        visible: control.opacity > 0.5
        anchors.fill: parent

        states: [
            State {
                when: control.position === ToolBar.Header
                ParentChange
                {
                    target: _overlay;
                    parent: _cask.overlayTopPanel ;
                }
            },

            State {
                when: control.position === ToolBar.Footer
                ParentChange
                {
                    target: _overlay; parent: _cask.overlayBottomPanel; x: 10; y: 10
                }
            }
        ]

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
            opacity: Math.min(0.7, control.opacity)

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

        MouseArea
        {
            anchors.fill: parent
            onClicked: {

                control.overlayClicked()
                mouse.accepted = false
            }
        }
    }

    Container
    {
        id: _cards
        anchors.fill: parent
        clip: true

        contentItem: ListView
        {
            id: _cardsList
            spacing: Maui.Style.space.medium
            model: _cards.contentModel
            snapMode: ListView.SnapOneItem
            orientation: ListView.Vertical
            //                interactive: Kirigami.Settings.hasTransientTouchInput
            boundsBehavior: ListView.StopAtBounds
        }

    }
}
