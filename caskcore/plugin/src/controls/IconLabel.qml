import QtQuick
import QtQuick.Controls
import org.mauikit.controls as Maui

Row
{
    id: control
    spacing: Maui.Style.space.small
    padding: 0
    property alias text : _label.text
    property alias label : _label

    property alias iconSource: _icon.source
    property alias icon : _icon
    property alias font : _label.font

    property alias labelVisible : _label.visible

    add: Transition {
          NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 400 }
          NumberAnimation { property: "scale"; from: 0; to: 1.0; duration: 400 }
      }

    Maui.Icon
    {
        id: _icon
        visible: String(_icon.source).length
        height: visible ? parent.height : 0
        width: height

        Behavior on color
        {
            ColorAnimation
            {
                easing.type: Easing.InQuad
                duration: Maui.Style.units.shortDuration
            }
        }
    }

    Label
    {
        id: _label
        height: visible ? parent.height : 0
        font.bold: true
        //        font.weight: Font.Light
        font.pointSize: Maui.Style.fontSizes.small
        color: _icon.color
        horizontalAlignment: Qt.AlignLeft
        verticalAlignment: Qt.AlignVCenter
        wrapMode: Text.NoWrap
        elide: Text.ElideRight

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
