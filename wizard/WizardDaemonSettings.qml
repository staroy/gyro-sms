// Copyright (c) 2014-2019, The Monero Project
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import QtQuick 2.9
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0

import "../js/Wizard.js" as Wizard
import "../components" as GyroComponents

ColumnLayout {
    Layout.fillWidth: true
    Layout.maximumWidth: wizardController.wizardSubViewWidth
    Layout.alignment: Qt.AlignHCenter
    spacing: 10

    function save(){
        persistentSettings.useRemoteNode = remoteNode.checked
        persistentSettings.remoteNodeAddress = remoteNodeEdit.getAddress();
        if (bootstrapNodeEdit.daemonAddrText == "auto") {
            persistentSettings.bootstrapNodeAddress = "auto";
        } else {
            persistentSettings.bootstrapNodeAddress = bootstrapNodeEdit.getAddress();
        }
    }

    GyroComponents.RadioButton {
        id: localNode
        Layout.fillWidth: true
        text: qsTr("Start a node automatically in background (recommended)") + translationManager.emptyString
        fontSize: 16
        checked: !appWindow.persistentSettings.useRemoteNode && !isAndroid && !isIOS
        visible: !isAndroid && !isIOS
        onClicked: {
            checked = true;
            remoteNode.checked = false;
        }
    }

    ColumnLayout {
        id: blockchainFolderRow
        visible: localNode.checked
        spacing: 20

        Layout.topMargin: 8
        Layout.fillWidth: true

        GyroComponents.LineEdit {
            id: blockchainFolder
            Layout.fillWidth: true

            readOnly: true
            labelText: qsTr("Blockchain location (optional)") + translationManager.emptyString
            labelFontSize: 14
            placeholderText: qsTr("Default") + translationManager.emptyString
            placeholderFontSize: 15
            text: persistentSettings.blockchainDataDir
            inlineButton.small: true
            inlineButtonText: qsTr("Browse") + translationManager.emptyString
            inlineButton.onClicked: {
                if(persistentSettings.blockchainDataDir != "");
                    blockchainFileDialog.folder = "file://" + persistentSettings.blockchainDataDir;
                blockchainFileDialog.open();
                blockchainFolder.focus = true;
            }
        }

        ColumnLayout{
            Layout.topMargin: 6
            spacing: 0

            Text {
                text: qsTr("Bootstrap node") + translationManager.emptyString
                Layout.topMargin: 10
                Layout.fillWidth: true
                font.family: GyroComponents.Style.fontRegular.name
                color: GyroComponents.Style.defaultFontColor
                font.pixelSize: {
                    if(wizardController.layoutScale === 2 ){
                        return 22;
                    } else {
                        return 16;
                    }
                }

                wrapMode: Text.WordWrap
                leftPadding: 0
                topPadding: 0
                bottomPadding: 0
            }

            Text {
                text: qsTr("Additionally, you may specify a bootstrap node to use Monero immediately.") + translationManager.emptyString
                Layout.topMargin: 4
                Layout.fillWidth: true

                font.family: GyroComponents.Style.fontRegular.name
                color: GyroComponents.Style.dimmedFontColor

                font.pixelSize: {
                    if(wizardController.layoutScale === 2 ){
                        return 16;
                    } else {
                        return 14;
                    }
                }

                wrapMode: Text.WordWrap
                leftPadding: 0
                topPadding: 0
                bottomPadding: 0
            }
        }

        ColumnLayout {
            spacing: 8
            Layout.fillWidth: true

            GyroComponents.RemoteNodeEdit {
                id: bootstrapNodeEdit
                Layout.minimumWidth: 300
                //labelText: qsTr("Bootstrap node (leave blank if not wanted)") + translationManager.emptyString

                initialAddress: persistentSettings.bootstrapNodeAddress
            }
        }
    }

    GyroComponents.RadioButton {
        id: remoteNode
        Layout.fillWidth: true
        Layout.topMargin: 8
        text: qsTr("Connect to a remote node") + translationManager.emptyString
        fontSize: 16
        checked: appWindow.persistentSettings.useRemoteNode
        onClicked: {
            checked = true
            localNode.checked = false
        }
    }

    ColumnLayout {
        visible: remoteNode.checked
        spacing: 0

        Layout.topMargin: 8
        Layout.fillWidth: true

        GyroComponents.RemoteNodeEdit {
            id: remoteNodeEdit
            Layout.fillWidth: true

            initialAddress: persistentSettings.remoteNodeAddress
        }
    }
}
