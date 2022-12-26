
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtQml 2.15

import QtWayland.Compositor 1.15
import QtGraphicalEffects 1.15

import org.mauikit.controls 1.3 as Maui

Rectangle
{
    id: _borders
    radius: _decoration.radius
    color: "transparent"
    border.color: Qt.darker(Maui.Theme.backgroundColor, 2.7)
    opacity: 0.8
    border.width: 1

    Rectangle
    {
        anchors.fill: parent
        anchors.margins: 1
        color: "transparent"
        radius: parent.radius - 0.5
        border.width: 1
        border.color: Qt.lighter(Maui.Theme.backgroundColor, 2)
        opacity: 0.4
    }

}
