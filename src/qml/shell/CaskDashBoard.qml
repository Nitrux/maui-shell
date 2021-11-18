import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask
import QtGraphicalEffects 1.0

Item
{
    id: control
    default property alias content: _content.data

    readonly property alias avaliableHeight : _content.height
    readonly property alias avaliableWidth : _content.width
    readonly property alias contentY: _content.y
    readonly property alias surface : _content

    property alias background : _rec

    property alias backgroundColor : _rec.color
    property alias backgroundImage: _img.source

    property alias topPanel: _topPanel
    property alias bottomPanel: _bottomPanel

    signal desktopPressed()

    readonly property bool rise : _dropArea.containsDrag

    Rectangle
    {
        id: _rec
        anchors.fill: parent
        color: "#333"

        Item
        {
            id: _bg
            anchors.fill: parent
            anchors.margins:  control.rise ? Maui.Style.space.large * 2 : 0

            Image
            {
                id: _img
                anchors.fill: parent
                fillMode: Image.PreserveAspectCrop
                layer.enabled: control.rise
                layer.effect: OpacityMask
                {
                    maskSource: Item
                    {
                        width: _img.width
                        height: _img.height

                        Rectangle
                        {
                            anchors.fill: parent
                            radius: Maui.Style.radiusV
                        }
                    }
                }

                Rectangle
                {
                    visible:  control.rise
                    anchors.fill: parent
                    //         anchors.margins: Maui.Style.space.small
                    radius: Maui.Style.radiusV
                    color: "transparent"
                    border.color: Qt.darker(Kirigami.Theme.backgroundColor, 2.7)
                    opacity: 0.5


                    Rectangle
                    {
                        anchors.fill: parent
                        anchors.margins: 1
                        color: "transparent"
                        radius: parent.radius - 0.5
                        border.color: Qt.lighter(Kirigami.Theme.backgroundColor, 2)
                        opacity: 0.7
                    }
                }
            }

        }

        DropShadow
        {
            visible: control.rise
            transparentBorder: true
            anchors.fill: _bg
            horizontalOffset: 0
            verticalOffset: 0
            radius: 8.0
            samples: 17
            color: Qt.rgba(0,0,0,0.9)
            source: _bg
        }

        DropArea
        {
            id: _dropArea

            anchors.fill: parent
            onDropped:
            {
                if(drop.urls)
                {
                    _img.source = drop.urls[0]
                }
            }
        }
    }

    ColumnLayout
    {
        anchors.fill: parent
        spacing: 0


        Cask.Panel
        {
            id: _topPanel
            z: _content.z+ 1
            Layout.fillWidth: true
            Layout.preferredHeight: visible ? implicitHeight : 0
        }

        Item
        {
            id: _container
            Layout.fillWidth: true
            Layout.fillHeight: true

            Item
            {
                id: _content
                anchors.fill: parent
            }

            //            TapHandler
            //            {
            //                grabPermissions: PointerHandler.CanTakeOverFromAnything
            //                gesturePolicy: TapHandler.ReleaseWithinBounds | TapHandler.WithinBounds
            //                onTapped:
            //                {
            //                    console.log("Dekstop tapped")
            //                    control.desktopPressed()
            //                }
            //            }

            MouseArea
            {
                anchors.fill: parent
                onPressed:
                {
                    control.desktopPressed()
                    mouse.accepted = false
                }
            }
        }

        Item
        {
            z: _topPanel.z+1
            //            color: "yellow"
            Layout.fillWidth: true
            Layout.preferredHeight: _bottomPanel.visible ? _bottomPanel.implicitHeight : 0
            Cask.Panel
            {
                id: _bottomPanel
                anchors.fill: parent
            }
        }
    }
}
