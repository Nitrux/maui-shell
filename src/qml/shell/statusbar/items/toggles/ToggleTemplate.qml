import QtQuick 2.13
import QtQml 2.14
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.3

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.3 as Maui
import "../../../templates"

CardButton
{
    id: control
    implicitHeight: 48 + topPadding + bottomPadding
    implicitWidth: Math.max(_template.layout.implicitWidth, implicitHeight) + leftPadding + rightPadding
    property alias template : _template

    display: ToolButton.IconOnly

    property Component page : null

    contentItem: Maui.ListItemTemplate
    {
        id: _template
        spacing: control.spacing
        labelsVisible: (label1.text.length || label2.text.length) && control.display === ToolButton.TextBesideIcon
        label1.text: control.text
        label1.wrapMode: Text.Wrap
        iconSource: control.icon.name
        isMask: false
        iconVisible: true
        iconSizeHint: Maui.Style.iconSizes.medium
//        headerSizeHint: control.implicitHeight
    }
}
