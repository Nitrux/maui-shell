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
Kirigami.Theme.inherit: false
Kirigami.Theme.colorSet: Kirigami.Theme.Complementary
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

    background:  Rectangle
    {
        color: Kirigami.Theme.backgroundColor
        opacity: win.formFactor !== Cask.Env.Desktop ? 0 : 0.7
        radius: 12
    }


    SwipeView
    {
        id: _swipeView
        anchors.fill: parent
        background: null

        Maui.GridView
        {
            id: _categoriesGridView
            itemSize: Math.min(150, Math.floor(flickable.width/2))
            itemHeight: 172
            model: _allAppsModel.groups

            verticalScrollBarPolicy: ScrollBar.AlwaysOff

            onAreaClicked: closeCard()

            delegate: Item
            {
                id: _delegate
                property string group: modelData.label
                width: GridView.view.cellWidth
                height: GridView.view.cellHeight

                Maui.GridBrowserDelegate
                {
                    height: parent.GridView.view.itemHeight- 10
                    width: parent.GridView.view.itemWidth-10
                    anchors.centerIn: parent
                    template.labelSizeHint: 22
                    iconSource:  modelData.icon
                    iconSizeHint: width
                    label1.font.bold: true
                    label1.font.weight: Font.ExtraBold
                    label1.text: modelData.label.replace("/", "")
                    background: null

                    onClicked:
                    {
                        _swipeView.incrementCurrentIndex()
                        _allAppsModel.group = modelData.label
                    }

                    template.iconComponent: Control
                    {
                        background: Rectangle
                        {
                            color: Kirigami.Theme.backgroundColor
                            opacity: 0.5
                            radius: 12
                        }

                        GridView
                        {
                            anchors.fill: parent
                            interactive: false
                            cellWidth: width/2
                            cellHeight: height/2

                            model: Maui.BaseModel
                            {
                                list: Cask.AppsModel
                                {
                                    group: modelData.label
                                    limit: 4
                                }
                            }

                            delegate: Item
                            {
                                height: GridView.view.cellHeight
                                width: GridView.view.cellWidth

                                Kirigami.Icon
                                {
                                    anchors.centerIn: parent
                                    height: 32
                                    width: height
                                    source: model.icon
                                }
                            }
                        }
                    }
                }
            }

        }

        Maui.GridView
        {
            id: _gridView
            itemSize: Math.min(150, Math.floor(flickable.width/3))

            model: Maui.BaseModel
            {
                filter: _searchBar.text
                sortOrder: Qt.DescendingOrder
                recursiveFilteringEnabled: true
                sortCaseSensitivity: Qt.CaseInsensitive
                filterCaseSensitivity: Qt.CaseInsensitive
                list: Cask.AppsModel
                {
                    id: _allAppsModel
                }
            }

            delegate: Item
            {
                width: GridView.view.cellWidth
                height: GridView.view.cellHeight

                Maui.GridBrowserDelegate
                {
                    height: parent.GridView.view.itemHeight- 10
                    width: parent.GridView.view.itemWidth-10
                    anchors.centerIn: parent

                    template.labelSizeHint: 22

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


    PageIndicator
    {
        id: pageIndicator
        interactive: true
        count: _swipeView.count
        currentIndex: _swipeView.currentIndex

        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
}


