import QtQuick 2.13
import QtQml 2.14
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.13
import QtGraphicalEffects 1.0

import org.kde.kirigami 2.7 as Kirigami
import org.kde.mauikit 1.2 as Maui

Item
{
    id: control
    property int suggestedHeight : 500

    property alias container : _cards
    property alias count: _cards.count
    property alias implicitHeight: _cardsList.contentHeight
    property alias contentChildren : _cards.contentChildren

    Rectangle
    {
        parent: surfaceArea
        anchors.fill: parent
        opacity: Math.min(0.7, control.height / cask.height)
        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
        color: Kirigami.Theme.backgroundColor
        radius: Maui.Style.radiusV
        //            border.color: Qt.tint(Kirigami.Theme.textColor, Qt.rgba(Kirigami.Theme.backgroundColor.r, Kirigami.Theme.backgroundColor.g, Kirigami.Theme.backgroundColor.b, 0.7))


    }

    ScrollView
    {
        anchors.fill: parent
        contentHeight: _cardsList.contentHeight
        ScrollBar.vertical.policy: ScrollBar.AlwaysOff

        Container
        {
            id: _cards
            anchors.fill: parent
            clip: true

            contentItem: ListView
            {
                id:_cardsList
                spacing: 0
                model: _cards.contentModel
                snapMode: ListView.SnapOneItem
                orientation: ListView.Vertical
                boundsBehavior: ListView.StopAtBounds

                TapHandler
                {
                    onTapped: close()
                }
            }
        }
    }
}