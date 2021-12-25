import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15

import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask
import QtGraphicalEffects 1.0

import QtQuick.Templates 2.15 as T

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

    headBar.middleContent: Maui.TextField
    {
        id: _searchBar
        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: control.Kirigami.Theme.colorSet
        Layout.maximumWidth: 500
        Layout.fillWidth: true
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
        layer.effect :  DropShadow
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
                        template.labelSizeHint: 22

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


