import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.2 as Maui

import org.maui.cask 1.0 as Cask

Maui.Page
{
    id: control

    headBar.background: null
    headBar.farLeftContent: ToolButton
    {
        icon.name: "go-previous"
        onClicked: control.StackView.view.pop()
    }

}
