import QtQuick 2.9
import QtQuick.Layouts 1.1
import FontAwesome 1.0

import "../components" as GyroComponents

RowLayout {
    id: advancedOptionsItem
    
    property alias title: title.text
    property alias button1: button1
    property alias button2: button2
    property alias button3: button3
    property alias helpTextLarge: helpTextLarge
    property alias helpTextSmall: helpTextSmall
      
    RowLayout {
        id: titlecolumn
        Layout.alignment: Qt.AlignTop | Qt.AlignLeft
        Layout.preferredWidth: 195
        Layout.maximumWidth: 195
        Layout.leftMargin: 10

        GyroComponents.Label {
            id: title
            fontSize: 14
        }

        GyroComponents.Label {
            id: iconLabel
            fontSize: 12
            text: FontAwesome.questionCircle
            fontFamily: FontAwesome.fontFamily
            opacity: 0.3
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: helpText.visible = !helpText.visible
                onEntered: parent.opacity = 0.4
                onExited: parent.opacity = 0.3
            }  
        }
        
        Rectangle {
            id: separator
            Layout.fillWidth: true
            height: 10
            color: "transparent"
        }
    }

    ColumnLayout {
        Layout.fillWidth: false
        Layout.alignment: Qt.AlignTop | Qt.AlignLeft
        spacing: 4
                
        RowLayout {
            Layout.fillWidth: false
            spacing: 12
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            
            StandardButton {
                id: button1
                small: true
                visible: button1.text
            }
    
            StandardButton {
                id: button2
                small: true
                visible: button2.text
            }
            
            StandardButton {
                id: button3
                small: true
                visible: button3.text
            }
        }
        
        ColumnLayout {
            id: helpText
            visible: false     
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            
            GyroComponents.TextPlain {
                id: helpTextLarge
                visible: helpTextLarge.text
                font.family: GyroComponents.Style.fontRegular.name
                font.pixelSize: 13
                color: GyroComponents.Style.defaultFontColor
            }
            
            GyroComponents.TextPlain {
                id: helpTextSmall
                visible: helpTextSmall.text
                Layout.leftMargin: 5
                textFormat: Text.RichText
                font.family: GyroComponents.Style.fontRegular.name
                font.pixelSize: 12
                color: GyroComponents.Style.defaultFontColor
            }
        }
    }
}
