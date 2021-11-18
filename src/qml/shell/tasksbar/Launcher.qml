import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask

Maui.Page
{
    id: control

    headBar.visible: true
    flickable: _gridView.flickable
    headBar.background: null
    headBar.middleContent: Maui.TextField
    {
        id: _searchBar
        Layout.maximumWidth: 500
        Layout.fillWidth: true
        placeholderText: qsTr("Search for apps and files...")
        onAccepted: _launcherGrid.model.filter = text
        onCleared: _launcherGrid.model.filter = ""
    }

    Cask.ProcessLauncher
    {
        id: launcher
    }

    background: null

    Maui.GridView
    {
        id: _gridView
        anchors.fill: parent
        itemSize: 150

        model: Maui.BaseModel
        {
            filter: _searchBar.text
            sortOrder: Qt.DescendingOrder
            recursiveFilteringEnabled: true
            sortCaseSensitivity: Qt.CaseInsensitive
            filterCaseSensitivity: Qt.CaseInsensitive
            list: Cask.AppsModel
            {

            }
        }

        delegate: Item
        {
            width: GridView.view.cellWidth
            height: GridView.view.cellHeight

            Maui.GridBrowserDelegate
            {
                anchors.fill: parent
                anchors.margins: Maui.Style.space.medium

                template.labelSizeHint: 42

                draggable: true
                Drag.keys: ["text/uri-list"]
                Drag.mimeData: { "text/uri-list": model.path }
                background: Item {}
                iconSource:  model.icon
                iconSizeHint: 64
                label1.text: model.label

                onClicked:
                {
                    console.log(model.executable)
                    launcher.launchApp(model.path)
                    closeCard()
                }
            }
        }

        function forceActiveFocus()
        {
            _searchBar.forceActiveFocus()
        }
    }
}


