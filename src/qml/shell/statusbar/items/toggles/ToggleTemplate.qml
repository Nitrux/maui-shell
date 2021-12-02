import QtQuick 2.13
import QtQml 2.14
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.3

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.3 as Maui

AbstractButton
{
    id: control
    implicitHeight: 64
    implicitWidth: Math.max(_template.implicitWidth, implicitHeight)

    property alias template : _template

    property Component page : null

    background: Rectangle
    {
        readonly property color m_color : Qt.tint(Qt.lighter(control.Kirigami.Theme.textColor), Qt.rgba(control.Kirigami.Theme.backgroundColor.r, control.Kirigami.Theme.backgroundColor.g, control.Kirigami.Theme.backgroundColor.b, 0.9))

        color: control.hovered ? control.Kirigami.Theme.hoverColor :( control.isCurrentItem ||  control.containsPress ? control.Kirigami.Theme.highlightColor : Qt.rgba(m_color.r, m_color.g, m_color.b, 0.4))

        radius: 10
    }

    contentItem: Maui.ListItemTemplate
    {
        id: _template
        labelsVisible: label1.text.length || label2.text.length
        label1.text: control.text
        iconSource: control.icon.name
        iconVisible: true
        iconSizeHint: Maui.Style.iconSizes.big
        headerSizeHint: control.implicitHeight

        Loader
        {
            asynchronous: true
            active: control.page
            visible: active
            Layout.fillHeight: true
            Layout.preferredWidth: height
            sourceComponent: Item
            {
                Kirigami.Icon
                {
                    color: Kirigami.Theme.textColor
                    anchors.centerIn: parent
                    source: "arrow-right"
                    height: 16
                    width: 16
                }
            }
        }
    }
}
