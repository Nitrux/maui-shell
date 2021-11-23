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

    Rectangle
    {

        visible: opacity > 0
        parent: _cask.background
        anchors.fill: parent
        opacity: Math.min(0.7, control.opacity)
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



    Container
    {
        id: _cards
        anchors.fill: parent
                    clip: true

        contentItem: ListView
            {
                id: _cardsList
                spacing: 0
                model: _cards.contentModel
                snapMode: ListView.SnapOneItem
                orientation: ListView.Vertical
//                interactive: Kirigami.Settings.hasTransientTouchInput
                boundsBehavior: ListView.StopAtBounds
            }

    }
}
