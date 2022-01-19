import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15

import org.kde.kirigami 2.7 as Kirigami

import org.mauikit.controls 1.3 as Maui
import org.mauikit.filebrowsing 1.0 as FB

import org.maui.cask 1.0 as Cask
import QtGraphicalEffects 1.0

import QtQuick.Templates 2.15 as T
import Qt.labs.platform 1.1

Maui.Page
{
    id: control
    focus: true

    opacity:  (y/finalYPos)

    readonly property int finalYPos :  0 - (control.height - _container.y)

    property bool opened : false

    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary

    property alias searchBar : _searchBar

    signal placeClicked(string path)

    Behavior on opacity
    {
        NumberAnimation
        {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutQuad
        }
    }

    headBar.visible: true
    headBar.background: null
    headBar.leftContent: ToolButton
    {
        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: control.Kirigami.Theme.colorSet

        icon.name: "go-previous"
        onClicked: _swipeView.decrementCurrentIndex()
        visible: _swipeView.currentIndex=== 1
    }

    headBar.middleContent: Maui.SearchField
    {
        id: _searchBar
        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: control.Kirigami.Theme.colorSet
        Layout.maximumWidth: 500
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignCenter
        placeholderText: qsTr("Search for apps and files...")
        onAccepted: _gridView.model.filter = text
        onCleared: _gridView.model.filter = ""
        onTextChanged:
        {
            if(_swipeView.currentIndex === 0)
                _swipeView.currentIndex = 1
        }

        Keys.onReturnPressed:
        {
            _gridView.forceActiveFocus()
        }

        //        Keys.forwardTo: _gridView
    }

    background: Rectangle
    {
        color: Kirigami.Theme.backgroundColor
        opacity: win.formFactor !== Cask.Env.Desktop ? 0 : 0.95
        radius: 10

        layer.enabled: win.formFactor === Cask.Env.Desktop
        layer.effect: DropShadow
        {
            transparentBorder: true
            horizontalOffset: 0
            verticalOffset: 0
            radius: 8.0
            samples: 17
            color: Qt.rgba(0,0,0,0.2)
        }
    }

    ColumnLayout
    {
        anchors.fill: parent

        SwipeView
        {
            id: _swipeView
            Layout.fillWidth: true
            Layout.fillHeight: true
            background: null
            clip: true

            ColumnLayout
            {
                spacing: Maui.Style.space.medium

                Maui.GridView
                {
                    id: _categoriesGridView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
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

                            template.iconComponent: T.Pane
                            {
                                background: Rectangle
                                {
                                    color:  Qt.lighter(Kirigami.Theme.backgroundColor)
                                    opacity: 0.5
                                    radius: 12
                                }

                                contentItem: GridView
                                {
                                    interactive: false
                                    cellWidth: width/2
                                    cellHeight: height/2
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

                                        Kirigami.Icon
                                        {
                                            anchors.centerIn: parent
                                            height: 32
                                            width: height
                                            source: model.icon
                                        }
                                    }
                                }

                                MouseArea
                                {
                                    anchors.fill: parent
                                    onClicked:
                                    {
                                        _swipeView.incrementCurrentIndex()
                                        _allAppsModel.group = modelData.label
                                    }
                                }
                            }
                        }
                    }

                    flickable.footer: T.Page
                    {
                        width: parent.width
                        implicitHeight: contentHeight+ topPadding + topPadding + header.height
//                        padding: Maui.Style.space.big
                        header: Item
                        {
                            width: parent.width
                            height: 60

                            Kirigami.Separator
                            {
                                anchors.centerIn: parent
                                width: 100
                                opacity: 0.5
                            }
                        }

                        contentItem: Flow
                        {

                            GridView
                            {
                                id: _quickSection
                                implicitHeight: contentHeight + topMargin +  bottomMargin
                                implicitWidth: 200
                                currentIndex :-1

                                cellWidth: Math.floor(width/3)
                                cellHeight: cellWidth
                                interactive: false

                                model: Maui.BaseModel
                                {
                                    list: FB.PlacesList
                                    {
                                        groups: [FB.FMList.QUICK_PATH, FB.FMList.PLACES_PATH]
                                    }
                                }

                                header: Maui.LabelDelegate
                                {
                                    width: parent.width
                                    isSection: true
                                    label: i18n("Places")
                                }

                                delegate: Item
                                {
                                    height: GridView.view.cellHeight
                                    width: GridView.view.cellWidth

                                    Maui.GridBrowserDelegate
                                    {
                                        isCurrentItem: parent.GridView.isCurrentItem
                                        anchors.fill: parent
                                        anchors.margins: Maui.Style.space.tiny
                                        iconSource: model.icon
                                        iconSizeHint: Maui.Style.iconSizes.medium
                                        template.isMask: true
                                        label1.text: model.label
                                        labelsVisible: false
                                        tooltipText: model.label
                                        onClicked:
                                        {
                                            placeClicked(model.path)
                                            FB.FM.openUrl(model.path)
                                        }

                                        //                            onRightClicked:
                                        //                            {
                                        //                                _menu.path = model.path
                                        //                                _menu.show()
                                        //                            }

                                        //                            onPressAndHold:
                                        //                            {
                                        //                                _menu.path = model.path
                                        //                                _menu.show()
                                        //                            }
                                    }
                                }
                            }

                            GridView
                            {
                                id: _recentSection
                                implicitHeight: contentHeight +  topMargin+  bottomMargin
                                implicitWidth: 300
                                currentIndex :-1

                                cellWidth: Math.floor(width/3)
                                cellHeight: cellWidth
                                interactive: false

                                model: Maui.BaseModel
                                {
                                    list: Cask.RecentFiles
                                    {
                                        url: StandardPaths.writableLocation(StandardPaths.DownloadLocation)
                                        filters: FB.FM.nameFilters(FB.FMList.IMAGE)

                                    }
                                }

                                header: Maui.LabelDelegate
                                {
                                    width: parent.width
                                    isSection: true
                                    label: i18n("Downloads")
                                }

                                delegate: Item
                                {
                                    height: GridView.view.cellHeight
                                    width: GridView.view.cellWidth

                                    Maui.GridBrowserDelegate
                                    {
                                        isCurrentItem: parent.GridView.isCurrentItem
                                        anchors.fill: parent
                                        anchors.margins: Maui.Style.space.tiny
                                        iconSource: model.icon
                                        imageSource: model.thumbnail
                                        iconSizeHint: Maui.Style.iconSizes.medium
                                        label1.text: model.label
                                        labelsVisible: !FB.FM.checkFileType(FB.FMList.IMAGE, model.mime)
                                        tooltipText: model.label
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
                focus: true
                itemSize: Math.min(150, Math.floor(flickable.width/3))
                currentIndex: 0

                holder.visible: _gridView.count === 0
                holder.title: i18n("Nothing here")
                holder.body: i18n("Try with a different query")
                holder.emoji: "anchor"

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

                onKeyPress:
                {
                    console.log("Key _pressed", event.key, _gridView.model.get(_gridView.currentIndex).executable)
                    if(event.key == Qt.Key_Return)
                    {
                        launchExec(_gridView.model.get(_gridView.currentIndex).executable)
                        control.close()
                    }
                }

                Maui.ContextualMenu
                {
                    id: _appMenu
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

                delegate: Item
                {
                    width: GridView.view.cellWidth
                    height: GridView.view.cellHeight

                    Maui.GridBrowserDelegate
                    {
                        height: parent.GridView.view.itemHeight- 10
                        width: parent.GridView.view.itemWidth-10
                        anchors.centerIn: parent
                        highlighted: parent.GridView.isCurrentItem
                        template.labelSizeHint: 44

                        draggable: true
                        Drag.keys: ["text/uri-list"]
                        Drag.mimeData: { "text/uri-list": model.path }
                        //                    background: Item {}
                        iconSource:  model.icon
                        iconSizeHint: 64
                        label1.text: model.label

                        onClicked:
                        {
                            console.log(model.executable)
                            launchExec(model.executable)
                            control.close()

                        }

                        onRightClicked:
                        {
                            _appMenu.show()
                        }
                    }
                }
            }
        }

        Item
        {
            Layout.fillWidth: true
            implicitHeight: 22
            PageIndicator
            {
                id: pageIndicator
                height: parent.height
                interactive: true
                count: _swipeView.count
                currentIndex: _swipeView.currentIndex

                anchors.centerIn: parent
            }
        }
    }

    DragHandler
    {
        id: handler3
        target: control
        dragThreshold: 60
        enabled: control.opened
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
        dock.forceActiveFocus()
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


