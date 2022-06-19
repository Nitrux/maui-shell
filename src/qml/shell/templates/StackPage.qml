import QtQuick 2.15
import QtQuick.Controls 2.15

import org.mauikit.controls 1.3 as Maui

Maui.Page
{
    id: control
    headerColorSet: control.Maui.Theme.colorSet
    showTitle: true
    padding: 0
    margins: 0
    headBar.background: null
    headBar.farLeftContent: ToolButton
    {
        icon.name: "go-previous"
        onClicked: control.StackView.view.pop()
    }

    background: null
}
