import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Templates 2.15 as T
import QtQuick.Layouts 1.12

import org.kde.kirigami 2.14 as Kirigami
import org.mauikit.controls 1.3 as Maui
import org.mauikit.filebrowsing 1.0 as FB

import org.maui.cask 1.0 as Cask

import QtGraphicalEffects 1.0

Maui.Dialog
{
    id: control
    defaultButtons: true
    hint: 1
    maxHeight: 800
    maxWidth: 600
    title: i18n("Wallpapers")
    closeButtonVisible: false
    acceptButton.text: _stackView.depth === 2 ? i18n("Set") : i18n("Accept")

    property string previousWallpaper

    onOpened:
    {
        previousWallpaper = wallpaperSettings.defaultWallpaper
    }

    onRejected:
    {
        wallpaperSettings.defaultWallpaper = control.previousWallpaper
        control.close()
    }

    onAccepted:
    {
        if(_stackView.depth === 2)
        {
            _stackView.pop()
            return
        }


        control.close()

    }

    headBar.leftContent: ToolButton
    {
        icon.name: "go-previous"
        visible: _stackView.depth > 1
        onClicked: _stackView.pop()
    }

    stack: StackView
    {
        id: _stackView
        Layout.fillWidth: true
        Layout.fillHeight: true
        implicitHeight: currentItem.implicitHeight

        initialItem: ScrollView
        {
            Flickable
            {
                contentWidth: availableWidth
                contentHeight: _initialLayout.implicitHeight

                ColumnLayout
                {
                    id: _initialLayout
                    width: parent.width
                    spacing: Maui.Style.space.medium

                    RowLayout
                    {
                        Layout.margins: Maui.Style.space.medium
                        Layout.fillWidth: true
                        Layout.fillHeight: false
                        Layout.preferredHeight: control.height*0.3
                        spacing: Maui.Style.space.big

                        ColumnLayout
                        {
                            Layout.fillHeight: true
                            Layout.fillWidth: true

                            Rectangle
                            {
                                id: _imgDesktop

                                Layout.fillHeight: true
                                Layout.fillWidth: true

                                radius: 8
                                color: "#333"

                                Image
                                {
                                    anchors.fill: parent
                                    fillMode: wallpaperSettings.fill ? Image.PreserveAspectCrop : Image.PreserveAspectFit
                                    source: wallpaperSettings.defaultWallpaper
                                }


                                Rectangle
                                {
                                    anchors.top: parent.top
                                    color: Kirigami.Theme.backgroundColor
                                    opacity: 0.7
                                    height: 10
                                    radius: 4
                                    width: parent.width* 0.95
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.margins: 6

                                }

                                Rectangle
                                {
                                    anchors.bottom: parent.bottom
                                    color: Kirigami.Theme.backgroundColor
                                    opacity: 0.7
                                    height: 30
                                    radius: 10
                                    width: parent.width* 0.6
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.margins: 6
                                }


                                layer.enabled: true
                                layer.effect: OpacityMask
                                {
                                    maskSource: Rectangle
                                    {
                                        width: _imgDesktop.width
                                        height: _imgDesktop.height
                                        radius: 8
                                    }
                                }
                            }


                            Button
                            {
                                Layout.alignment: Qt.AlignCenter
                                Layout.fillWidth: true
                                text: i18n("Desktop")
                                onClicked: _stackView.push(_wallpapersGridViewComponent)
                            }

                        }

                        ColumnLayout
                        {

                            Layout.fillHeight: true
                            Layout.fillWidth: true

                            Rectangle
                            {
                                id: _imgLockScreen

                                Layout.fillHeight: true
                                Layout.fillWidth: true
                                radius: 8
                                color: "#333"

                                Image
                                {
                                    anchors.fill: parent
                                    fillMode: wallpaperSettings.fill ? Image.PreserveAspectCrop : Image.PreserveAspectFit
                                    source: wallpaperSettings.defaultWallpaper
                                }


                                Column
                                {

                                    anchors.centerIn: parent
                                    spacing: Maui.Style.space.medium

                                    Rectangle
                                    {
                                        color: Kirigami.Theme.backgroundColor
                                        opacity: 0.7
                                        implicitHeight: 80
                                        implicitWidth: height
                                        radius: 20
                                    }

                                    Rectangle
                                    {
                                        color: Kirigami.Theme.backgroundColor
                                        opacity: 0.7
                                        implicitHeight: 20
                                        implicitWidth: 120
                                        radius: 4
                                    }
                                }



                                layer.enabled: true
                                layer.effect: OpacityMask
                                {
                                    maskSource: Rectangle
                                    {
                                        width: _imgLockScreen.width
                                        height: _imgLockScreen.height
                                        radius: 8
                                    }
                                }
                            }

                            Button
                            {
                                Layout.alignment: Qt.AlignCenter
                                Layout.fillWidth: true
                                text: i18n("Lock Screen")
                                onClicked: _stackView.push(_wallpapersGridViewComponent)
                            }
                        }
                    }

                    Maui.SettingsSection
                    {
                        title: i18n("Options")

                        Maui.SettingTemplate
                        {
                            label1.text: i18n("Dim Wallpaper")
                            label2.text: i18n("On dark mode the wallpaper will be dimmed")

                            Switch
                            {
                                checked: wallpaperSettings.dim
                                onToggled: wallpaperSettings.dim = !wallpaperSettings.dim
                            }
                        }

                        Maui.SettingTemplate
                        {
                            label1.text: i18n("Fill Wallpaper")
                            label2.text: i18n("Fill or fit wallpaper")

                            Switch
                            {
                                checked: wallpaperSettings.fill
                                onToggled: wallpaperSettings.fill = !wallpaperSettings.fill
                            }
                        }
                    }

                    Maui.SettingsSection
                    {
                        title: i18n("Customize")

                        Maui.SettingTemplate
                        {
                            label1.text: i18n("Solid Color")
                            label2.text: i18n("Pick a solid color")

                            Maui.ColorsRow
                            {
                            }
                        }

                        Maui.SettingTemplate
                        {
                            label1.text: i18n("Widgets")
                            label2.text: i18n("Show widgets")

                            Switch
                            {
                            }
                        }
                    }


                }
            }
        }
        Component
        {
            id: _wallpapersGridViewComponent

            Maui.GridView
            {
                id: _wallpapersGridView

                //            implicitHeight: contentHeight
                currentIndex :-1

                itemSize: Math.floor(width/3)

                model: Maui.BaseModel
                {
                    list: Cask.RecentFiles
                    {
                        url: wallpaperSettings.sourcePath
                        filters: FB.FM.nameFilters(FB.FMList.IMAGE)
                        limit: 40
                    }
                }

                delegate: Item
                {
                    height: GridView.view.cellHeight
                    width: GridView.view.cellWidth
                    property string wallpaper : model.url
                    Maui.GridBrowserDelegate
                    {
                        isCurrentItem: parent.GridView.isCurrentItem
                        anchors.fill: parent
                        anchors.margins: Maui.Style.space.tiny
                        iconSource: model.icon
                        imageSource: model.thumbnail
                        //                    iconSizeHint: Maui.Style.iconSizes.medium
                        label1.text: model.label
                        //                    template.labelSizeHint: 32
                        labelsVisible:false
                        tooltipText: model.label

                        onClicked:
                        {
                            _wallpapersGridView.currentIndex = index
                            wallpaperSettings.defaultWallpaper = model.thumbnail
                        }

                        onDoubleClicked:
                        {
                            _wallpapersGridView.currentIndex = index
                            wallpaperSettings.defaultWallpaper = model.thumbnail
                            _stackView.pop()
                        }
                    }
                }
            }
        }
    }
}

