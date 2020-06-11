import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.7 as Kirigami
import org.kde.mauikit 1.2 as Maui

Item
{
    id: control
    property alias content: _content.data

    readonly property alias avaliableHeight : _avaliableRec.height
    readonly property alias avaliableWidth : _avaliableRec.width
    readonly property alias contentY: _avaliableRec.y
    readonly property alias surface : _avaliableRec

    property alias backgroundColor : _rec.color
    property alias backgroundImage: _img.source

    property alias topPanel: _topPanel
    property alias bottomPanel: _bottomPanel

    signal desktopPressed(var mouse)

    Rectangle
    {
        id: _rec
        anchors.fill: parent
        Image
        {
            id: _img
            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop
        }
    }

    ColumnLayout
    {
        anchors.fill: parent

        Panel
        {
            id: _topPanel
            Layout.fillWidth: true
            Layout.preferredHeight: implicitHeight
        }

        Item
        {
            id: _content
            Layout.fillWidth: true
            Layout.fillHeight: true

            MouseArea
            {
                anchors.fill: parent
                id: _avaliableRec
                propagateComposedEvents: true
                preventStealing: false
                onPressed: control.desktopPressed(mouse)
            }
        }

        Panel
        {
            id: _bottomPanel
            Layout.fillWidth: true
            Layout.preferredHeight: implicitHeight

        }
    }
}
