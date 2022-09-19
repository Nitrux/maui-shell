import QtQuick 2.15
import QtQml 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.15

import org.mauikit.controls 1.2 as Maui
import org.maui.cask 1.0 as Cask

Container
{
    id: control
    spacing: Maui.Style.space.medium
    implicitHeight: _listView.implicitHeight + topPadding+bottomPadding
    contentItem: ListView
    {
        id: _listView
        implicitHeight: contentHeight
        spacing: control.spacing
        ToolButton
        {
            text: "add"
            onClicked: control.pushOSD(_component)

        }
        model: control.contentModel

        add: Transition {
               NumberAnimation { properties: "x,y"; from: 100; duration: 1000 }
           }

        addDisplaced: Transition {
               NumberAnimation { properties: "x,y"; duration: 1000 }
           }

        remove: Transition {
              ParallelAnimation {
                  NumberAnimation { property: "opacity"; to: 0; duration: 1000 }
                  NumberAnimation { properties: "x,y"; to: 100; duration: 1000 }
              }
          }

        removeDisplaced: Transition {
               NumberAnimation { properties: "x,y"; duration: 1000 }
           }

        populate: Transition {
                NumberAnimation { properties: "x,y"; duration: 1000 }
            }
    }

    Component
    {
        id: _component

        Control
        {
            id: _delegate
            property int mindex : index
            Maui.Theme.colorSet: Maui.Theme.Complementary
            padding: Maui.Style.space.medium

            height: implicitContentHeight+topPadding+bottomPadding
            width: 300

            hoverEnabled: true

            background: Rectangle
            {
                color: Maui.Theme.backgroundColor
                radius: Maui.Style.radiusV

                layer.enabled: true
                layer.effect :  DropShadow
                {
                    horizontalOffset: 0
                    verticalOffset: 0
                    color: Qt.rgba(0,0,0,0.6)
                    radius: 12
                    samples: 25
                }
            }

            contentItem: Column
            {
                RowLayout
                {
                    width: parent.width
                    spacing: Maui.Style.space.big

                    Rectangle
                    {
                        color: "pink"
                        implicitHeight: 64
                        implicitWidth: 64
                    }

                    Rectangle
                    {
                        color: "blue"
                        Layout.fillWidth: true
                        implicitHeight: 100
                    }
                }
            }

            Timer
            {
                id: _timer
                interval: 5500
                repeat: false
                //                triggeredOnStart: true

                onTriggered:
                {
                    if(_delegate.hovered)
                    {
                        _timer.restart()
                        return
                    }

                    control.removeItem(control.itemAt(_delegate.mindex))
                }
            }

            Component.onCompleted: _timer.start()
        }
    }

    function pushOSD(component)
    {
        var item = component.createObject(control)
        control.addItem(item)
    }
}
