import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import Ossia 1.0 as Ossia

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Item {
        id: root
        anchors.fill: parent
        Ossia.Node { node: "/"; id: rootnode }
        Column
        {
            Repeater {
                model: Ossia.Instances {
                    node: "leText";
                    count: 5;
                    parentNode: rootnode;
                    onCountChanged: console.log("count: " + count)
                }
                delegate: Text {
                    font.pointSize: 20
                    color: Qt.hsla(0.8 / (1+index), 1/ (1+index), 0.7 , 1)
                    text: "Index: " + index

                    Ossia.Node { node: "leText." + index }
                    Ossia.Property on text { }
                    Ossia.Property on font.pointSize { }
                    //Ossia.Property on color { }

                    Component.onCompleted: console.log("creating text")
                    Component.onDestruction: console.log("destroying text")
                }
            }
        }
    }
    Component.onCompleted:
    {
        //Ossia.SingleDevice.rescan(root)
        //Ossia.SingleDevice.savePreset("file:///tmp/preset.json")
        console.log("loading")
        Ossia.SingleDevice.setReadPreset(true);
        Ossia.SingleDevice.loadPreset(root, "file:///tmp/preset.json")
        console.log("loaded")
    }
}