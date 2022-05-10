import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.3 as Maui

Row
{
    id: control
    spacing: Maui.Style.space.medium

    property alias text : _label.text
    property alias label : _label

    property alias iconSource: _icon.source
    property alias icon : _icon

    property alias labelVisible : _label.visible

    Kirigami.Icon
    {
        id: _icon
        implicitHeight: parent.height
        implicitWidth: height
    }

    Label
    {
        id: _label
        height: parent.height
    }
}
