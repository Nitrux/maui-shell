import QtQuick 2.15
import QtQuick.Controls 2.15

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.3 as Maui

Maui.Page
{
    id: control
    headerColorSet: control.Kirigami.Theme.colorSet
    showTitle: true

    headBar.background: null
    headBar.farLeftContent: ToolButton
    {
        icon.name: "go-previous"
        onClicked: control.StackView.view.pop()
    }
}
