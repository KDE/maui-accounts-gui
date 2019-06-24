import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

import org.mauikit.accounts 1.0 as Accounts

Item {
    property alias swipeView: swipeView;
    property string appId;
    signal accountAdded(string secret);

    SwipeView {
        id: swipeView
        interactive: false
        anchors.fill: parent
        currentIndex: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                anchors.centerIn: parent
                Button {
                    text: "OpenDesktop"
                    Layout.minimumWidth: 250
                    onClicked: {
                        inputServer.text = ""
                        inputServer.visible = false
                        buttonAddAccountOpendesktop.visible = true
                        buttonAddAccountCustom.visible = false

                        swipeView.setCurrentIndex(1)
                    }
                }
                Button {
                    text: "Custom Server"
                    Layout.minimumWidth: 250
                    onClicked: {
                        inputServer.visible = true
                        buttonAddAccountOpendesktop.visible = false
                        buttonAddAccountCustom.visible = true

                        swipeView.setCurrentIndex(1)
                    }
                }
            }
        }

        Rectangle {
            id: rectangle
            Layout.fillWidth: true
            Layout.fillHeight: true

            RoundButton {
                width: Qt.platform.os == "Android" ? 64 : 48
                height: Qt.platform.os == "Android" ? 64 : 48
                icon.source: "icons/back.png"
                z: 100
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 16
                anchors.right: parent.right
                anchors.rightMargin: 16
                visible: swipeView.currentIndex > 0
                onClicked: {
                    backPressed()
                }
            }

            ColumnLayout {
                anchors.top: parent.top
                anchors.topMargin: 16
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.right: parent.right
                anchors.rightMargin: 16

                ComboBox {
                    id: inputProtocol
                    Layout.fillWidth: true
                    textRole: "value"
                    visible: false

                    model: ListModel {
                        id: inputProtocolModel
                        ListElement {
                            key: "carddav"
                            value: "CardDAV"
                        }
                        ListElement {
                            key: "caldav"
                            value: "CalDAV"
                        }
                    }
                }

                TextField {
                    id: inputServer
                    Layout.fillWidth: true
                    placeholderText: "Server Address"
                }

                TextField {
                    id: inputUsername
                    Layout.fillWidth: true
                    placeholderText: "Username"
                }

                TextField {
                    id: inputPassword
                    Layout.fillWidth: true
                    placeholderText: "Password"
                    echoMode: TextInput.PasswordEchoOnEdit
                }

                Button {
                    id: buttonAddAccountOpendesktop
                    text: qsTr("Add Account")
                    Layout.minimumWidth: 250
                    Layout.alignment: Qt.AlignHCenter
                    onClicked: {
                        Accounts.Controller.addOpendesktopAccount(appId, inputProtocolModel.get(inputProtocol.currentIndex).key, inputUsername.text, inputPassword.text)
                    }
                }

                Button {
                    id: buttonAddAccountCustom
                    text: qsTr("Add Account")
                    Layout.minimumWidth: 250
                    Layout.alignment: Qt.AlignHCenter
                    onClicked: {
                        Accounts.Controller.addCustomAccount(appId, inputProtocolModel.get(inputProtocol.currentIndex).key, inputServer.text, inputUsername.text, inputPassword.text)
                    }
                }
            }
        }

        Connections {
            target: Accounts.Controller

            onAccountAdded: {
                swipeView.setCurrentIndex(0);

                accountAdded(secret);

                inputServer.text = ""
                inputUsername.text = ""
                inputPassword.text = ""
            }
        }
    }

    function backPressed() {
        if (swipeView.currentIndex > 0) {
            close.accepted = false
            swipeView.setCurrentIndex(swipeView.currentIndex-1)
        }
    }
}



/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
