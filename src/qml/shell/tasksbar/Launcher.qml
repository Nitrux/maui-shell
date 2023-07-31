import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.mauikit.controls as Maui
import org.mauikit.filebrowsing as FB

import org.maui.cask as Cask

import Qt5Compat.GraphicalEffects

import Qt.labs.platform as Labs

Maui.Page
{
    id: control
    objectName: "LauncherPage"

    opacity: (y/finalYPos)
    property bool fullscreenLauncher: false // win.formFactor !== Cask.MauiMan.Desktop

    readonly property int finalYPos :  0 - (control.height)

    property bool opened : false

    property alias searchBar : _searchBar

    signal placeClicked(string path)

    Keys.enabled: true
    Keys.onEscapePressed: control.close()

    Behavior on opacity
    {
        NumberAnimation
        {
            duration: Maui.Style.units.shortDuration
            easing.type: Easing.InOutQuad
        }
    }

    Maui.ContextualMenu
    {
        id: _appMenu
        property var app : ({})

        title: app.label
        subtitle: app.comment
        titleIconSource: app.icon

        MenuItem
        {
            text: i18n("Pin")
        }

        MenuItem
        {
            text: i18n("Launch")
        }

        MenuItem
        {
            text: i18n("Uninstall")
        }

        MenuItem
        {
            text: i18n("Information")
        }

        MenuItem
        {
            text: i18n("Hide")
        }
    }

    Cask.AppsModel
    {
        id: _allAppsModel
    }

    headBar.visible: true
    headBar.background: null
    headBar.forceCenterMiddleContent: !isMobile
    headBar.leftContent: ToolButton
    {
        icon.name: "go-previous"
        onClicked: _stackView.pop()
        visible: _stackView.depth >1
    }

    headBar.middleContent: Maui.SearchField
    {
        id: _searchBar
        Layout.maximumWidth: 500
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignCenter
        placeholderText: qsTr("Search for apps and files...")
        onAccepted: _gridView.model.filter = text
        onCleared: _gridView.model.filter = ""
        onTextChanged:
        {
            if(_stackView.depth === 1)
                _stackView.push(_appsListComponent)
        }

        Keys.onReturnPressed:
        {
            _gridView.forceActiveFocus()
        }

        //        Keys.forwardTo: _gridView
    }

    background: Rectangle
    {
        visible: !control.fullscreenLauncher
        color: Maui.Theme.backgroundColor

        radius: Maui.Style.radiusV

        Behavior on color
        {
            ColorAnimation
            {
                easing.type: Easing.InQuad
                duration: Maui.Style.units.shortDuration
            }
        }

        layer.enabled: !control.fullscreenLauncher
        layer.effect: DropShadow
        {
            horizontalOffset: 0
            verticalOffset: 0
            samples: 10
            color: Qt.rgba(0,0,0,0.5)
        }
    }

    SwipeView
    {
        anchors.fill: parent
        background: null
        padding: Maui.Style.space.medium

        StackView
        {
            id: _stackView

            //            Layout.fillWidth: true
            //            Layout.preferredHeight: currentItem.implicitHeight
            //            Layout.maximumHeight: parent.height
            //            Layout.alignment: Qt.AlignCenter
            background: null
            clip: true

            initialItem: ColumnLayout
            {
                spacing: Maui.Style.space.medium
                property alias atYBeginning : _categoriesGridView.flickable.atYBeginning

                Maui.GridBrowser
                {
                    id: _categoriesGridView
                    objectName: "LauncherGrid"

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    itemSize: Math.min(150, Math.floor(flickable.width/3))
                    itemHeight: itemSize
                    model: _allAppsModel.groups

                    verticalScrollBarPolicy: ScrollBar.AlwaysOff

                    onAreaClicked: control.close()

                    delegate: Item
                    {
                        id: _delegate
                        property string group: modelData.label
                        width: GridView.view.cellWidth
                        height: GridView.view.cellHeight

                        Maui.GridBrowserDelegate
                        {
                            height: parent.GridView.view.itemHeight-10
                            width: parent.GridView.view.itemWidth-10
                            anchors.centerIn: parent
                            template.labelSizeHint: 22
                            iconSizeHint: width
                            label1.font.bold: true
                            label1.font.weight: Font.ExtraBold
                            label1.text: modelData.label.replace("/", "")
                            background: null

                            onClicked:
                            {
                                _stackView.push(_appsListComponent)
                                _allAppsModel.group = modelData.label
                            }

                            template.iconComponent: Pane
                            {
                                padding: Maui.Style.space.small
                                background: Rectangle
                                {
                                    color: Maui.Theme.alternateBackgroundColor

                                    radius: Maui.Style.radiusV

                                    Behavior on color
                                    {
                                        ColorAnimation
                                        {
                                            easing.type: Easing.InQuad
                                            duration: Maui.Style.units.shortDuration
                                        }
                                    }
                                }

                                contentItem: GridView
                                {
                                    interactive: false
                                    cellWidth: switch(count)
                                               {
                                               case 4:
                                               case 3:
                                               case 2: return width/2;
                                               case 1: return width
                                               default: return width/2;
                                               }

                                    cellHeight: switch(count)
                                                {
                                                case 4:
                                                case 3: return height/2;
                                                case 2:
                                                case 1: return height
                                                default: return height/2;
                                                }


                                    clip: true
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

                                        Maui.Icon
                                        {
                                            anchors.centerIn: parent
                                            property int size: Math.min(parent.width, parent.height)
                                            height: width
                                            width: Maui.Style.mapToIconSizes(size*0.8)
                                            source: model.icon
                                        }
                                    }
                                }

                                MouseArea
                                {
                                    anchors.fill: parent
                                    onClicked:
                                    {
                                        _stackView.push(_appsListComponent)
                                        _allAppsModel.group = modelData.label
                                    }
                                }
                            }
                        }
                    }

                    flickable.header: Maui.SectionGroup
                    {
                        width: parent.width
                        height: visible ? implicitHeight : 0
                        visible: _recentListView.count > 0
                        title: i18n("Most Used")
                        padding: Maui.Style.space.medium

                        ListView
                        {
                            id: _recentListView
                            orientation: ListView.Horizontal
                            Layout.fillWidth: true
                            Layout.preferredHeight: 140
                            spacing: Maui.Style.space.medium
                            model: _appsDB.recentApps

                            delegate: Maui.GridBrowserDelegate
                            {
                                height: ListView.view.height
                                width: height

                                iconSource: model.icon
                                label1.text: model.name

                                iconSizeHint: 64
                                template.labelSizeHint: 44

                                onClicked:
                                {
                                    _appsDB.launchApp(model.path)
                                    control.close()
                                }
                            }
                        }

                        Maui.Separator
                        {
                            Layout.preferredWidth: 100
                            //                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignCenter
                        }
                    }

                }
            }

            Component
            {
                id: _appsListComponent

                Maui.GridBrowser
                {
                    id: _gridView
                    property bool atYBeginning : _gridView.flickable.atYBeginning

                    focus: true
                    itemSize: Math.min(120, Math.floor(flickable.width/3))
                    currentIndex: 0

                    holder.visible: _gridView.count === 0
                    holder.title: i18n("Nothing here")
                    holder.body: i18n("Try with a different query")
                    holder.emoji: "anchor"

                    model: Maui.BaseModel
                    {
                        id: _allAppsBaseModel
                        filter: _searchBar.text
                        sortOrder: Qt.AscendingOrder
                        sort: "label"
                        recursiveFilteringEnabled: true
                        sortCaseSensitivity: Qt.CaseInsensitive
                        filterCaseSensitivity: Qt.CaseInsensitive
                        list: _allAppsModel
                    }

                    onKeyPress:
                    {
                        console.log("Key _pressed", event.key, _gridView.model.get(_gridView.currentIndex).executable)
                        if(event.key == Qt.Key_Return)
                        {
                            _appsDB.launchApp(_gridView.model.get(_gridView.currentIndex).path)
                            control.close()
                        }
                    }

                    delegate: Item
                    {
                        width: GridView.view.cellWidth
                        height: GridView.view.cellHeight

                        Maui.GridBrowserDelegate
                        {
                            height: parent.GridView.view.itemHeight-10
                            width: parent.GridView.view.itemWidth-10
                            anchors.centerIn: parent
                            highlighted: parent.GridView.isCurrentItem

                            draggable: true
                            Drag.keys: ["text/uri-list"]
                            Drag.mimeData: { "text/uri-list": model.path }
                            //                    background: Item {}
                            iconSource:  model.icon
                            label1.text: model.label

                            iconSizeHint: 64
                            template.labelSizeHint: 40

                            onClicked:
                            {
                                _gridView.currentIndex = index
                                _appsDB.launchApp(model.path)
                                control.close()
                            }

                            onRightClicked:
                            {
                                _gridView.currentIndex = index
                                _appMenu.app = _allAppsBaseModel.get(index)
                                _appMenu.show()
                            }
                        }
                    }
                }
            }
        }

//        Page
//        {
//            padding: Maui.Style.space.big
//            background: null

//            contentItem: Flow
//            {

//                GridView
//                {
//                    id: _quickSection
//                    implicitHeight: contentHeight
//                    implicitWidth: 200
//                    currentIndex :-1

//                    cellWidth: Math.floor(width/3)
//                    cellHeight: cellWidth
//                    interactive: false

//                    model: Maui.BaseModel
//                    {
//                        list: FB.PlacesList
//                        {
//                            groups: [FB.FMList.QUICK_PATH, FB.FMList.PLACES_PATH]
//                        }
//                    }

//                    header: Maui.LabelDelegate
//                    {
//                        width: parent.width
//                        isSection: true
//                        label: i18n("Places")
//                    }

//                    delegate: Item
//                    {
//                        height: GridView.view.cellHeight
//                        width: GridView.view.cellWidth

//                        Maui.GridBrowserDelegate
//                        {
//                            isCurrentItem: parent.GridView.isCurrentItem
//                            anchors.fill: parent
//                            anchors.margins: Maui.Style.space.tiny
//                            iconSource: model.icon
//                            iconSizeHint: Maui.Style.iconSizes.medium
//                            template.isMask: true
//                            label1.text: model.label
//                            labelsVisible: false
//                            tooltipText: model.label
//                            onClicked:
//                            {
//                                placeClicked(model.path)
//                                FB.FM.openUrl(model.path)
//                            }
//                        }
//                    }
//                }

//                GridView
//                {
//                    id: _recentSection
//                    implicitHeight: contentHeight
//                    implicitWidth: 300
//                    currentIndex :-1

//                    cellWidth: Math.floor(width/3)
//                    cellHeight: cellWidth
//                    interactive: false

//                    model: Maui.BaseModel
//                    {
//                        list: Cask.RecentFiles
//                        {
//                            url: Labs.StandardPaths.writableLocation(Labs.StandardPaths.DownloadLocation)
//                            //                                        filters: FB.FM.nameFilters(FB.FMList.IMAGE)

//                        }
//                    }

//                    header: Maui.LabelDelegate
//                    {
//                        width: parent.width
//                        isSection: true
//                        label: i18n("Downloads")
//                    }

//                    delegate: Item
//                    {
//                        height: GridView.view.cellHeight
//                        width: GridView.view.cellWidth

//                        Maui.GridBrowserDelegate
//                        {
//                            isCurrentItem: parent.GridView.isCurrentItem
//                            anchors.fill: parent
//                            anchors.margins: Maui.Style.space.tiny
//                            iconSource: model.icon
//                            imageSource: model.thumbnail
//                            iconSizeHint: Maui.Style.iconSizes.medium
//                            label1.text: model.label
//                            template.labelSizeHint: 32
//                            labelsVisible: !FB.FM.checkFileType(FB.FMList.IMAGE, model.mime)
//                            tooltipText: model.label
//                        }
//                    }
//                }

//                Maui.GalleryRollItem
//                {
//                    implicitHeight: 220
//                    implicitWidth: 280
//                    images: _pictures.urls

//                    Cask.RecentFiles
//                    {
//                        id: _pictures
//                        url: Labs.StandardPaths.writableLocation(Labs.StandardPaths.PicturesLocation)
//                        filters: FB.FM.nameFilters(FB.FMList.IMAGE)

//                    }
//                }
//            }
//        }

    }
    DragHandler
    {
        id: handler3
        target: control
        dragThreshold: 60
        enabled: control.opened && _stackView.currentItem.atYBeginning
        yAxis.minimum: control.finalYPos

        xAxis.enabled : false
        grabPermissions: PointerHandler.CanTakeOverFromItems | PointerHandler.CanTakeOverFromHandlersOfDifferentType | PointerHandler.ApprovesTakeOverByAnything

        onActiveChanged:
        {
            const condition = (handler3.centroid.scenePosition.y - handler3.centroid.scenePressPosition.y > 200)
            if(!active && condition)
            {
                control.close()
            }else
            {
                control.open()
            }
        }
    }



    function forceActiveFocus()
    {
        _searchBar.forceActiveFocus()
        _searchBar.selectAll()
    }

    function close()
    {
        _launcher.opened = false
        _launcher.y = control.parent.height
        workspaces.currentItem.forceActiveFocus()
//        dock.forceActiveFocus()
    }

    function open()
    {
        _launcher.opened = true
        _launcher.y = _launcher.finalYPos
        _launcher.forceActiveFocus()
    }

    function toggle()
    {
        if(_launcher.opened)
            _launcher.close()
        else
            _launcher.open()
    }
}


