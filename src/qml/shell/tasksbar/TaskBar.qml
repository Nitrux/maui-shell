import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.7 as Kirigami
import org.kde.mauikit 1.2 as Maui
import org.maui.cask 1.0 as Cask
import org.cask.env 1.0 as Env

import com.theqtcompany.wlprocesslauncher 1.0

Cask.PanelSection
{
    id: control
    Layout.fillWidth: true
    //    backgroundColor: "transparent"
    spacing: Maui.Style.space.medium

    preferredHeight: win.formFactor === Env.Env.Phone ? Maui.Style.toolBarHeight : Maui.Style.toolBarHeightAlt
    margins:  win.formFactor === Env.Env.Desktop ? Maui.Style.space.medium : 0
    radius:  win.formFactor === Env.Env.Desktop ? Maui.Style.radiusV : 0

    leftContent:[ Cask.PanelItem
        {
            Layout.fillWidth: false
            implicitWidth: height
            Layout.fillHeight: true
            icon.name: "view-list-icons"
            visible: !isMobile

            onClicked:
            {
                control.open(card.index)
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
                        autoExclusive: true
                        expanded: isWide
                        currentIndex : control.viewType === Maui.AltBrowser.ViewType.List ? 0 : 1
                        enabled: list.count > 0
                        display: ToolButton.TextBesideIcon
                        Action
                        {
                            text: i18n("List")
                            icon.name: "view-list-details"
                            onTriggered: _launcherGrid.viewType = Maui.AltBrowser.ViewType.List
                        }

                        Action
                        {
                            text: i18n("Grid")
                            icon.name: "view-list-icons"
                            onTriggered: _launcherGrid.viewType= Maui.AltBrowser.ViewType.Grid
                        }
                    }

                    Maui.AltBrowser
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
                        viewType: Maui.AltBrowser.ViewType.Grid
                        gridView.itemSize: 100
                        gridView.margins: Maui.Style.space.medium

                        gridDelegate: Maui.ItemDelegate
                        {
                            width: _launcherGrid.gridView.cellWidth
                            height: _launcherGrid.gridView.cellHeight

                            draggable: true
                            Drag.keys: ["text/uri-list"]
                            Drag.mimeData: { "text/uri-list": model.path }
                            background: Item {}

                            Maui.GridItemTemplate
                            {
                                height: _launcherGrid.gridView.itemSize
                                width: height
                                anchors.centerIn: parent
                                iconSource:  model.icon
                                iconSizeHint: 48
                                label1.text: model.label
                            }

                            onClicked:
                            {
                                console.log(model.executable)
                                //                                                        launcher.launch(model.executable)
                                Maui.KDE.launchApp(model.path)
                                control.close()
                            }
                        }

                        listDelegate: Maui.ItemDelegate
                        {
                            width: parent.width
                            height: 48
                            leftPadding: Maui.Style.space.small
                            rightPadding: Maui.Style.space.small
                            Maui.ListItemTemplate
                            {
                                anchors.fill: parent
                                iconSource:  model.icon
                                iconSizeHint: 32
                                label1.text: model.label
                                label2.text: model.comment
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
        },

        Cask.PanelItem
        {
            Layout.fillWidth: false
            implicitWidth: height
            Layout.fillHeight: true
            icon.name: "window-copy"
            visible: !isMobile
            checked: overView
            onClicked: overView = true
        }]

    ListModel {id: _tasksModel}
//    ListModel {id: _runninTasksModel}

    middleContent:  [ Repeater
        {
            model: _tasksModel

            Maui.ItemDelegate
            {
                Layout.fillHeight: true
                implicitWidth: height

                draggable: true


                Kirigami.Icon
                {
                    source: model.icon
                    height: isMobile ? 32 :22
                    width: height
                    anchors.centerIn: parent
                }
            }
        },
        Repeater
        {
            model: _listSurfaces

            Maui.ItemDelegate
            {
                Layout.fillHeight: true
                implicitWidth: height
                draggable: true

                onClicked: _swipeView.currentIndex = index

                Kirigami.Icon
                {
                    source: Env.Env.appIconName(modelData.toplevel.appId)
                    height: isMobile ? 32 :22
                    width: height
                    anchors.centerIn: parent
                }

                Rectangle
                {
                    width: parent.width
                    height: 2
                    anchors.bottom: parent.bottom
                    visible: index === _swipeView.currentIndex
                    color: Kirigami.Theme.highlightColor
                }
            }
        }
    ]

    onContentDropped:
    {
        console.log("Dropped things" , drop.urls)
        _tasksModel.append({icon: "vvave", title: "Apps Title", id : "appId", path: "desktopFIle"})
    }
}
