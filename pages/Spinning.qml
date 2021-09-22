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
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import "../components" as GyroComponents
import gyroComponents.Wallet 1.0

Rectangle {
    id: root
    color: "transparent"
    property alias spinningHeight: mainLayout.height
    property double currentHashRate: 0

    ColumnLayout {
        id: mainLayout
        Layout.fillWidth: true
        anchors.margins: 20
        anchors.topMargin: 40
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        spacing: 20

        GyroComponents.Label {
            id: soloTitleLabel
            fontSize: 24
            text: qsTr("Solo spinning") + translationManager.emptyString
        }

        GyroComponents.WarningBox {
            Layout.bottomMargin: 8
            text: qsTr("Spinning is only available on local daemons.") + translationManager.emptyString
            visible: persistentSettings.useRemoteNode
        }

        GyroComponents.WarningBox {
            Layout.bottomMargin: 8
            text: qsTr("Your daemon must be synchronized before you can start spinning") + translationManager.emptyString
            visible: !persistentSettings.useRemoteNode && !appWindow.daemonSynced
        }

        GyroComponents.TextPlain {
            id: soloMainLabel
            text: qsTr("Spinning with your computer helps strengthen the Monero network. The more that people mine, the harder it is for the network to be attacked, and every little bit helps.\n\nSpinning also gives you a small chance to earn some Monero. Your computer will create hashes looking for block solutions. If you find a block, you will get the associated reward. Good luck!") + translationManager.emptyString
            wrapMode: Text.Wrap
            Layout.fillWidth: true
            font.family: GyroComponents.Style.fontRegular.name
            font.pixelSize: 14
            color: GyroComponents.Style.defaultFontColor
        }

        GyroComponents.WarningBox {
            id: warningLabel
            Layout.topMargin: 8
            Layout.bottomMargin: 8
            text: qsTr("Spinning is not may reduce the performance of other running applications and processes.") + translationManager.emptyString
        }

        GridLayout {
            columns: 2
            Layout.fillWidth: true
            columnSpacing: 20
            rowSpacing: 16

            /*ColumnLayout {
                Layout.fillWidth: true
                Layout.alignment : Qt.AlignTop | Qt.AlignLeft

                GyroComponents.Label {
                    id: soloMinerThreadsLabel
                    color: GyroComponents.Style.defaultFontColor
                    text: qsTr("CPU threads") + translationManager.emptyString
                    fontSize: 16
                    wrapMode: Text.WordWrap
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 16

                GyroComponents.LineEdit {
                    id: soloMinerThreadsLine
                    Layout.minimumWidth: 200
                    text: "1"
                    validator: IntValidator { bottom: 1; top: idealThreadCount }
                }

                GyroComponents.TextPlain {
                    id: numAvailableThreadsText
                    text: qsTr("Max # of CPU threads available for spinning: ") + idealThreadCount + translationManager.emptyString
                    wrapMode: Text.WordWrap
                    font.family: GyroComponents.Style.fontRegular.name
                    font.pixelSize: 14
                    color: GyroComponents.Style.defaultFontColor
                }

                RowLayout {
                    GyroComponents.StandardButton {
                        id: autoRecommendedThreadsButton
                        small: true
                        text: qsTr("Use recommended # of threads") + translationManager.emptyString
                        enabled: startSoloMinerButton.enabled
                        onClicked: {
                                soloMinerThreadsLine.text = Math.floor(idealThreadCount / 2);
                                appWindow.showStatusMessage(qsTr("Set to use recommended # of threads"),3)
                        }
                    }

                    GyroComponents.StandardButton {
                        id: autoSetMaxThreadsButton
                        small: true
                        text: qsTr("Use all threads") + translationManager.emptyString
                        enabled: startSoloMinerButton.enabled
                        onClicked: {
                            soloMinerThreadsLine.text = idealThreadCount
                            appWindow.showStatusMessage(qsTr("Set to use all threads") + translationManager.emptyString,3)
                        }
                    }
                }

                RowLayout {
                    GyroComponents.CheckBox {
                        id: backgroundSpinning
                        enabled: startSoloMinerButton.enabled
                        checked: persistentSettings.allow_background_spinning
                        onClicked: {persistentSettings.allow_background_spinning = checked}
                        text: qsTr("Background spinning (experimental)") + translationManager.emptyString
                    }
                }

                RowLayout {
                    // Disable this option until stable
                    visible: false
                    GyroComponents.CheckBox {
                        id: ignoreBattery
                        enabled: startSoloMinerButton.enabled
                        checked: !persistentSettings.spinningIgnoreBattery
                        onClicked: {persistentSettings.spinningIgnoreBattery = !checked}
                        text: qsTr("Enable spinning when running on battery") + translationManager.emptyString
                    }
                }
            }*/

            ColumnLayout {
                Layout.alignment : Qt.AlignTop | Qt.AlignLeft

                GyroComponents.Label {
                    id: manageSoloMinerLabel
                    color: GyroComponents.Style.defaultFontColor
                    text: qsTr("Manage spinner") + translationManager.emptyString
                    fontSize: 16
                    wrapMode: Text.Wrap
                    Layout.preferredWidth: manageSoloMinerLabel.textWidth
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 16

                RowLayout {
                    GyroComponents.StandardButton {
                        visible: true
                        id: startSoloMinerButton
                        small: true
                        text: qsTr("Start spinning") + translationManager.emptyString
                        onClicked: {
                            var success = walletManager.startSpinning(appWindow.currentWallet.address(0, 0), 1 /*soloMinerThreadsLine.text*/, false /*persistentSettings.allow_background_spinning*/, true /*persistentSettings.spinningIgnoreBattery*/)
                            if (success) {
                                update()
                            } else {
                                errorPopup.title  = qsTr("Error starting spinning") + translationManager.emptyString;
                                errorPopup.text = qsTr("Couldn't start spinning.<br>") + translationManager.emptyString
                                if (persistentSettings.useRemoteNode)
                                    errorPopup.text += qsTr("Spinning is only available on local daemons. Run a local daemon to be able to mine.<br>") + translationManager.emptyString
                                errorPopup.icon = StandardIcon.Critical
                                errorPopup.open()
                            }
                        }
                    }

                    GyroComponents.StandardButton {
                        visible: true
                        id: stopSoloMinerButton
                        small: true
                        text: qsTr("Stop spinning") + translationManager.emptyString
                        onClicked: {
                            walletManager.stopSpinning()
                            update()
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.alignment : Qt.AlignTop | Qt.AlignLeft

                GyroComponents.Label {
                    id: statusLabel
                    color: GyroComponents.Style.defaultFontColor
                    text: qsTr("Status") + translationManager.emptyString
                    fontSize: 16
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 16

                GyroComponents.LineEditMulti {
                    id: statusText
                    Layout.minimumWidth: 300
                    text: qsTr("Not spinning") + translationManager.emptyString
                    borderDisabled: true
                    readOnly: true
                    wrapMode: Text.Wrap
                    inputPaddingLeft: 0
                }
            }
        }
    }

    function updateStatusText() {
        if (appWindow.isSpinning) {
            var userHashRate = walletManager.spinningHashRate();
            if (userHashRate === 0) {
                statusText.text = qsTr("Spinning temporarily suspended.") + translationManager.emptyString;
            }
            else {
                var blockTime = 120;
                var blocksPerDay = 86400 / blockTime;
                var globalHashRate = walletManager.networkDifficulty() / blockTime;
                var probabilityFindNextBlock = userHashRate / globalHashRate;
                var probabilityFindBlockDay = 1 - Math.pow(1 - probabilityFindNextBlock, blocksPerDay);
                var chanceFindBlockDay = Math.round(1 / probabilityFindBlockDay);
                statusText.text = qsTr("Spinning at %1 H/s. It gives you a 1 in %2 daily chance of finding a block.").arg(userHashRate).arg(chanceFindBlockDay) + translationManager.emptyString;
            }
        }
        else {
            statusText.text = qsTr("Not spinning") + translationManager.emptyString;
        }
    }

    function onSpinningStatus(isSpinning) {
        var daemonReady = !persistentSettings.useRemoteNode && appWindow.daemonSynced
        appWindow.isSpinning = isSpinning;
        updateStatusText()
        startSoloMinerButton.enabled = !appWindow.isSpinning && daemonReady
        stopSoloMinerButton.enabled = !startSoloMinerButton.enabled && daemonReady
    }

    function update() {
        walletManager.spinningStatusAsync();
    }

    GyroComponents.StandardDialog {
        id: errorPopup
        cancelVisible: false
    }

    Timer {
        id: timer
        interval: 2000; running: false; repeat: true
        onTriggered: update()
    }

    function onPageCompleted() {
        console.log("Spinning page loaded");
        update()
        timer.running = !persistentSettings.useRemoteNode
    }

    function onPageClosed() {
        timer.running = false
    }

    Component.onCompleted: {
        walletManager.spinningStatus.connect(onSpinningStatus);
    }
}
