import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.7 as Kirigami
import org.kde.mauikit 1.2 as Maui
import org.maui.cask 1.0 as Cask
import com.theqtcompany.wlprocesslauncher 1.0

Cask.PanelSection
{
    id: control
    Layout.fillWidth: true
//    backgroundColor: "transparent"
    spacing: Maui.Style.space.medium

    Cask.PanelItem
    {
        icon.name: "view-list-icons"
        onClicked:
        {
            control.open()
            _searchBar.forceActiveFocus()
        }

        card: Cask.PanelCard
        {
            width: parent.width
            height: 500
            padding: 0

            Maui.Page
            {
                anchors.fill: parent
                anchors.margins: 1
                headBar.visible: true
                margins: 10
                flickable: _launcherGrid.flickable
                headBar.middleContent: Maui.TextField
                {
                    id: _searchBar
                    Layout.fillWidth: true
                    placeholderText: qsTr("Search for apps and files...")
                    onAccepted: _launcherGrid.model.filter = text
                    onCleared: _launcherGrid.model.filter = ""
                }

                ProcessLauncher {
                    id: launcher
                }

                background: Rectangle
                {
                    color: Kirigami.Theme.backgroundColor
                    opacity: 0.7
                    radius: Maui.Style.radiusV
                }

                headBar.leftContent: Maui.ToolActions
                {
                    expanded: root.isWide
                    autoExclusive: true

                    Action
                    {
                        icon.name: "view-list-icons"
                        text: qsTr("Grid")
                    }

                    Action
                    {
                        icon.name: "view-list-details"
                        text: qsTr("List")
                    }
                }

                Maui.GridView
                {
                    id: _launcherGrid
                    anchors.fill: parent
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

                    adaptContent: true

                    itemSize: 100

                    verticalScrollBarPolicy:  Qt.ScrollBarAlwaysOff
                    delegate: Maui.ItemDelegate
                    {
                        width: _launcherGrid.cellWidth
                        height: _launcherGrid.cellHeight

                        Maui.GridItemTemplate
                        {
                            height: _launcherGrid.itemSize
                            width: _launcherGrid.itemSize
                            anchors.centerIn: parent
                            iconSource:  model.icon
                            iconSizeHint: 48
                            label1.text: model.label
                        }

                        onClicked:
                        {
                            console.log(model.executable)
//                            launcher.launch(model.executable)
                             Maui.KDE.launchApp(model.path)
                            control.close()
                        }
                    }
                }
            }
        }
    }

    Cask.PanelItem
    {
        icon.name: "application-menu"
    }

    Kirigami.Separator
    {
        Layout.fillHeight: true
    }

    Row
    {
        Layout.fillWidth: true
        spacing: Maui.Style.space.big

        Repeater
        {
            model: ["index", "vvave", "nota", "buho", "pix"]

            Kirigami.Icon
            {
                source:modelData
                height: 22
                width: height
            }
        }
    }
}
