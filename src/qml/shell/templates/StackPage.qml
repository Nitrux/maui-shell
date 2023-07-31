import QtQuick
import QtQuick.Controls

import org.mauikit.controls 1.3 as Maui

Maui.Page
{
    id: control

    showTitle: false

    padding: 0
    topMargin: Maui.Style.contentMargins

    headBar.padding: 0
    headBar.background: null

    headBar.farLeftContent: ToolButton
    {
        icon.name: "go-previous"
        onClicked: control.StackView.view.pop()
    }

    background: null
}
