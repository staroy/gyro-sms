import QtQuick 2.9
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import FontAwesome 1.0

import "../components" as GyroComponents
import "../components/effects/" as GyroEffects

import gyroComponents.Clipboard 1.0
import gyroComponents.Wallet 1.0
import gyroComponents.WalletManager 1.0
import gyroComponents.TransactionHistory 1.0
import gyroComponents.TransactionHistoryModel 1.0
import "../js/TxUtils.js" as TxUtils

Rectangle {
    color: "transparent"

    property alias smsHeight: mainLayout.height
    property string currentAddress: ""

    /* main layout */
    ColumnLayout {
        id: mainLayout
        Layout.fillWidth: true
        anchors.margins: 0
        anchors.topMargin: 0

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right

        spacing: 0

        GridLayout {
            Layout.topMargin: 0
            columns: 2
            rows: 3
            columnSpacing: 20

            ColumnLayout {

                RowLayout { GyroComponents.LineEdit {
                    id: addressFilter
                    Layout.fillWidth: true
                    fontSize: 16
                    labelFontSize: 14
                    labelText: " "
                    placeholderFontSize: 16
                    placeholderText: qsTr("Address filter") + "..." + translationManager.emptyString
                    readOnly: false
                    onTextChanged: {
                        currentWallet.addressBookModel.setFilterString(text);
                    }
                }}
            
                RowLayout {
                    id: addressListRow
                    property int addressListItemHeight: 50
                    Layout.fillWidth: true
                    Layout.preferredHeight: addressListItemHeight * addressListView.count + addressListItemHeight
                    ListView {
                        id: addressListView
                        Layout.topMargin: 12
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        boundsBehavior: ListView.StopAtBounds
                        interactive: false
                        delegate: Rectangle {
                            id: addressListRrect
                            height: addressListRow.addressListItemHeight
                            width: addressListView.width
                            Layout.fillWidth: true
                            color: selected ? (GyroComponents.Style.blackTheme ? "#505050" : "#b0b0b0") : "transparent"

                            readonly property bool selected: address == currentAddress

                            Rectangle {
                                color: GyroComponents.Style.appWindowBorderColor
                                anchors.right: parent.right
                                anchors.left: parent.left
                                anchors.top: parent.top
                                height: 1
                        
                                GyroEffects.ColorTransition {
                                    targetObj: parent
                                    blackColor: GyroComponents.Style._b_appWindowBorderColor
                                    whiteColor: GyroComponents.Style._w_appWindowBorderColor
                                }
                            }
                        
                            Rectangle {
                                anchors.fill: parent
                                anchors.topMargin: 5
                                //anchors.rightMargin: 110
                                color: "transparent"

                                GyroComponents.Label {
                                    id: labelLabel
                                    color: GyroComponents.Style.defaultFontColor
                                    //anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 6
                                    fontSize: 16
                                    text: label
                                    elide: Text.ElideRight
                                }
                        
                                Label {
                                    id: unreadCntLabel
                                    y: 0
                                    x: addressListRrect.width - 18 - dummyUnreadCnt.paintedWidth
                                    height: 22
                                    width: 12 + dummyUnreadCnt.paintedWidth
                                    padding: 3
                                    color: GyroComponents.Style.defaultFontColor
                                    font.pointSize: 8
                                    horizontalAlignment: Qt.AlignCenter
                                    text: unreadCount;
                                    visible: unreadCount > 0
                                    background: Rectangle {
                                        radius: 10
                                        color: selected ? (GyroComponents.Style.blackTheme ? "#909090" : "White") : (GyroComponents.Style.blackTheme ? "#505050" : "#b0b0b0")
                                    }
                                }

                                Text {
                                    id: dummyUnreadCnt
                                    visible: false
                                    text: unreadCount
                                }

                                GyroComponents.Label {
                                    id: addressLabel
                                    y: 22
                                    color: GyroComponents.Style.defaultFontColor
                                    //anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 6 /*labelLabel.width - parent.width + */
                                    fontSize: 16
                                    fontFamily: GyroComponents.Style.fontMonoRegular.name;
                                    text: TxUtils.addressTruncatePretty(address, mainLayout.width < 540 ? 1 : (mainLayout.width < 700 ? 2 : 3));
                                }
                            }
                            MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton
                                onClicked: {
                                    currentAddress = address
                                    currentWallet.smsModel.set_current(address, label)
                                }
                            }
                        }
                        ScrollBar.vertical: ScrollBar {}
                    }
                }
                RowLayout {
                }
            }

            ColumnLayout {
                RowLayout { GyroComponents.LineEdit {
                    id: smsFilter
                    Layout.fillWidth: true
                    fontSize: 16
                    labelFontSize: 14
                    labelText: " "
                    placeholderFontSize: 16
                    placeholderText: qsTr("sms filter") + "..." + translationManager.emptyString
                    readOnly: false
                    onTextChanged: {
                        currentWallet.smsModel.setFilterString(text);
                    }
                }}


                RowLayout {
                    id: smsListRow
                    property int smsListItemHeight: 20
                    Layout.fillWidth: true
                    Layout.preferredHeight: appWindow.height - 300
                    ListView {
                        id: smsListView
                        Layout.topMargin: 12
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        width: smsListRow.width
                        clip: true
                        boundsBehavior: ListView.StopAtBounds
                        delegate: Rectangle {
                            height: smsListRow.smsListItemHeight + 22 * (dummyText.paintedWidth / (smsFilter.width - 2))
                            Layout.fillWidth: true
                            color: "transparent"

                            Rectangle {
                                color: GyroComponents.Style.appWindowBorderColor
                                anchors.right: parent.right
                                anchors.left: parent.left
                                anchors.top: parent.top
                                height: 1

                                GyroEffects.ColorTransition {
                                    targetObj: parent
                                    blackColor: GyroComponents.Style._b_appWindowBorderColor
                                    whiteColor: GyroComponents.Style._w_appWindowBorderColor
                                }
                            }

                            Rectangle {
                                id: textArea
                                anchors.fill: parent
                                anchors.topMargin: 5
                                anchors.rightMargin: 110
                                color: "transparent"
                                property string fullTextSMS: "[" + time + "] " + (sentByMe ? "<< " : ">> ")+ textSMS

                                GyroComponents.LineEditMulti {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 0
                                    inputPaddingLeft: 0
                                    inputPaddingRight: 0
                                    inputPaddingTop: 0
                                    inputPaddingBottom: 0
                                    width: smsFilter.width
                                    borderDisabled: true
                                    fontSize: 16
                                    text: textArea.fullTextSMS
                                    readOnly: true
                                    wrapMode: Text.WordWrap
                                    fontColor: sentByMe ? (GyroComponents.Style.blackTheme ? "#999999" : "#777777") : GyroComponents.Style.defaultFontColor
                                }

                                Text {
                                    id: dummyText
                                    visible: false
                                    text: textArea.fullTextSMS
                                }
                            }
                        }
                        ScrollBar.vertical: ScrollBar {}
                        onCountChanged:  {
                            currentIndex = count-1
                        }
                    }
                }

                RowLayout { GyroComponents.LineEdit {
                    id: smsSendEditor
                    Layout.fillWidth: true
                    fontSize: 16
                    labelFontSize: 14
                    labelText: " "
                    placeholderFontSize: 16
                    placeholderText: qsTr("SMS") + "..." + translationManager.emptyString
                    readOnly: false
                    enabled: currentAddress.length > 0
                    Keys.onPressed: {
                        if(event.key === Qt.Key_Return) {
                            currentWallet.smsModel.append(
                              smsSendEditor.text.substring(0, smsSendEditor.text.length))
                            event.accepted = true;
                        }
                    }
                    Keys.onReleased: {
                        if(event.key === Qt.Key_Return) smsSendEditor.text = ""
                    }
                }}
            }
        }
    }
    
    function onPageCompleted() {
        console.log("SMS");
        addressListView.model = currentWallet.addressBookModel;
        addressFilter.text = currentWallet.addressBookModel.getFilterString();
        smsListView.model = currentWallet.smsModel;
    }
    
    function onPageClosed() {
    }
}
