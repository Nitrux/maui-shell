import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

import org.kde.kirigami 2.7 as Kirigami
import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask
import Zpaces 1.0 as ZP
import QtQuick.Templates 2.15 as T

T.Control
{
    id: control

    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.Complementary

    Layout.fillWidth: false
    Layout.alignment: Qt.AlignCenter
    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)

    Layout.margins: Maui.Style.space.medium
    implicitHeight: 64
    padding: Maui.Style.space.small

    ListModel {id: _tasksModel}

    background: Rectangle
    {
        color: Kirigami.Theme.backgroundColor
        opacity: 0.8
        radius: 10
    }

    contentItem: Cask.PanelSection
    {
        id: _section

        position: ToolBar.Footer
        Layout.fillHeight: true
        //    Layout.fillWidth: false

        //    Layout.preferredWidth: 400
        //    Layout.maximumWidth: _cask.avaliableWidth
        //    backgroundColor: "transparent"
        spacing: Maui.Style.space.medium
        alignment: Qt.AlignCenter

        popWidth: 800

        LauncherPanelItem
        {
            height: parent.height

            onClicked:
            {
                if(_section.popup.opened)
                    _section.close()
                else
                {
                    _section.open(card.index)
                    forceActiveFocus()
                }
            }
        }

        Cask.PanelItem
        {
            iconSize: 48
            height: parent.height
            leftPadding: 0
            rightPadding: 0
            visible: !isMobile
            background: Rectangle
            {
                color: Qt.darker(Kirigami.Theme.backgroundColor)
                radius: 10
            }

            Item
            {
                Layout.preferredHeight: 48
                Layout.preferredWidth: height


                Kirigami.Icon
                {
                    source: "view-file-columns"
                    color: Kirigami.Theme.textColor
                    height: 22
                    width: height
                    anchors.centerIn: parent
                }
            }

            //            visible: !isMobile
            checked: overView
            onClicked: overView = true
        }

        //    ListModel {id: _runninTasksModel}

        Repeater
        {
            model: _tasksModel

            Maui.ItemDelegate
            {
                implicitWidth: height
                implicitHeight: 48
                draggable: true

                Kirigami.Icon
                {
                    source: model.icon
                    height: isMobile ? 32 : 22
                    width: height
                    anchors.centerIn: parent
                }
            }
        }

        Repeater
        {
            model: _zpaces.tasksModel

            AbstractButton
            {

                property ZP.XdgWindow xdgWindow : model.window
                implicitHeight: 48
                implicitWidth: height
                //                draggable: true
                ToolTip.text: xdgWindow.title
                ToolTip.delay: 1000
                ToolTip.timeout: 5000
                ToolTip.visible: hovered


                onClicked:
                {
//                    var toggleMinimize = false
//                    if(_swipeView.currentIndex === index)
//                    {
//                        toggleMinimize = true
//                    }

                    _swipeView.currentIndex = model.zpaceIndex

//                    if(toggleMinimize)
//                    {
//                        _swipeView.itemAtIndex(index).chrome.visible = !_swipeView.itemAtIndex(index).chrome.visible
//                    }
                }

                contentItem: Item
                {
                    Kirigami.Icon
                    {
                        source: model.window.iconName
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



            //    onContentDropped:
            //    {
            //        console.log("Dropped things" , drop.urls)
            //        _tasksModel.append({icon: "vvave", title: "Apps Title", id : "appId", path: "desktopFIle"})
            //    }
        }

    }

    function closeCard()
    {
        _section.close()
    }
}
