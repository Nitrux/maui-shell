import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask

Cask.PanelSection
{
    id: control
//    Layout.fillWidth: false
    Layout.alignment: Qt.AlignCenter
//    Layout.preferredWidth: 400
    Layout.maximumWidth: _cask.avaliableWidth
    //    backgroundColor: "transparent"
    spacing: Maui.Style.space.medium

    preferredHeight: 64
    margins: 20
    radius:  20
    popWidth: Math.min(_cask.avaliableWidth, 800)

    Connections
    {
        target: _cask

        function onDesktopPressed()
        {
            console.log("DEsktop pressed")
            control.close()
        }
    }

    leftContent:[
        LauncherPanelItem
        {
            onClicked:
            {
                if(popup.visible)
                    control.close()
                else
                {
                    control.open(card.index)
                    forceActiveFocus()
                }
            }
        },

        Cask.PanelItem
        {
            icon.name: "cs-workspaces"
            iconSize: 48
            //            visible: !isMobile
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
                    height: isMobile ? 32 : 22
                    width: height
                    anchors.centerIn: parent
                }
            }
        },

        Repeater
        {
            model: _listSurfaces

            AbstractButton
            {
                Layout.fillHeight: true
                implicitWidth: height
                //                draggable: true

                onClicked:
                {
                    var toggleMinimize = false
                    if(_swipeView.currentIndex === index)
                    {
                        toggleMinimize = true
                    }

                    _swipeView.currentIndex = index

                    if(toggleMinimize)
                    {
                        _swipeView.itemAtIndex(index).chrome.visible = !_swipeView.itemAtIndex(index).chrome.visible
                    }
                }

                contentItem: Item
                {
                    Kirigami.Icon
                    {
                        source: Cask.Env.appIconName(modelData.toplevel.appId)
                        height: 48
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
        }
    ]

    onContentDropped:
    {
        console.log("Dropped things" , drop.urls)
        _tasksModel.append({icon: "vvave", title: "Apps Title", id : "appId", path: "desktopFIle"})
    }

    function closeCard()
    {
        control.close()
    }
}
