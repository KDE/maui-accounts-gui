import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import org.mauikit.accounts 1.0 as Accounts
import "utils.js" as Utils

ApplicationWindow {
    id: home
    width: 1280
    height: 720
    visible: true
    header: ToolBar {
        id: menuBar
        width: parent.width
        height: Utils.isAndroid() ? 64 : 48

        Label {
            id: labelAppName
            text: "Accounts"
            font.pointSize: Utils.isAndroid() ? 24 : 12
            verticalAlignment: Text.AlignVCenter
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.verticalCenter: parent.verticalCenter
        }

        RoundButton {
            padding: 0
            flat: true
            icon.source: "icons/refresh.png"
            display: AbstractButton.IconOnly
            anchors.right: parent.right
            anchors.rightMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            visible: stackPages.currentIndex == 0

            onClicked: {
                Accounts.Controller.getAccountList()
            }
        }
    }
    onClosing: {
        if (Utils.isAndroid()) {
            backPressed()
        } else {
            close.accepted = true
        }
    }

    Dialog {
        id: passwordDialog

        standardButtons: StandardButton.Ok | StandardButton.Cancel
        onAccepted: {
            Accounts.Controller.setMasterPassword(passwordInput.text)
            passwordInput.text = ""

            Accounts.Controller.getAccountList()
        }

        ColumnLayout {
            Text {
                text: "Enter Master password to Encrypt/Decrypt secure data"
                font.pointSize: 10
            }

            TextField {
                Layout.fillWidth: parent
                Layout.topMargin: 16
                Layout.bottomMargin: 16
                id: passwordInput
                Layout.preferredHeight: 35
                placeholderText: "Password"
            }
        }
    }

    Popup {
        id: progressDialog
        anchors.centerIn: parent
        width: 600
        height: 100
        modal: true
        focus: true
        closePolicy: Popup.NoAutoClose

        Rectangle {
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 20
            anchors.left: parent.left

            AnimatedImage {
                id: spinner
                height: 50
                anchors.verticalCenter: parent.verticalCenter
                fillMode: Image.PreserveAspectFit
                source: "icons/spinner.gif"
            }

            Label {
                id: progressDialogText
                anchors.left: spinner.right
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

    ListModel {
        id: listmodelAccounts
    }

    StackLayout {
        id: stackPages
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.top: menuBar.bottom
        anchors.topMargin: 0

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Label {
                id: labelNoAccounts
                color: "#999999"
                text: qsTr("No Accounts Found. Add Account to get started.")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 24
                font.italic: true
                font.weight: Font.Light
                visible: listmodelAccounts.count <= 0
            }

            ScrollView {
                anchors.fill: parent

                ColumnLayout {
                    id: accountsList
                    width:parent.width

                    anchors.left: parent.left
                    anchors.leftMargin: 24
                    anchors.top: parent.top
                    anchors.topMargin: 24
                    anchors.right: parent.right
                    anchors.rightMargin: 24

                    spacing: 16

                    Item {
                        Layout.fillWidth: true
                        Layout.bottomMargin: 24
                        visible: listmodelAccounts.count > 0

                        Label {
                            id: labelCarddav
                            color: "#888888"
                            text: "CardDAV"

                            anchors.top: parent.top
                        }

                        Rectangle {
                            height: 2
                            color: "#cccccc"
                            radius:4
                            anchors.top: labelCarddav.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right
                        }
                    }

                    Repeater {
                        model: listmodelAccounts

                        Item {
                            id: listmodelAccountsDelegate
                            Layout.fillWidth: true
                            height: 80

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    Accounts.Controller.showUrl(listmodelAccounts.get(index).url)
                                }
                            }

                            Menu {
                                id: accountsMenu

                                MenuItem {
                                    text: "Sync Now"
                                    onClicked: {
                                         Accounts.Controller.syncAccount(listmodelAccounts.get(index).appId)
                                    }
                                }

                                MenuItem {
                                    text: "Remove"
                                    onClicked: {
                                        Accounts.Controller.removeAccount(listmodelAccounts.get(index).id)
                                    }
                                }
                            }

                            Rectangle {
                                color: "#f8f8f8"
                                radius:4
                                anchors.fill: parent

                                ColumnLayout {
                                    anchors.right: parent.right
                                    anchors.rightMargin: 16
                                    anchors.left: parent.left
                                    anchors.leftMargin: 16
                                    anchors.verticalCenter: parent.verticalCenter

                                    Text {
                                        text: host
                                        font.pointSize: Utils.isAndroid() ? 14 : 10
                                        font.bold: true
                                    }
                                    Text {
                                        text: username
                                    }
                                }

                                RoundButton {
                                    flat: true
                                    icon.source: "icons/menu.png"
                                    display: AbstractButton.IconOnly
                                    anchors.right: parent.right
                                    anchors.rightMargin: 16
                                    anchors.verticalCenter: parent.verticalCenter

                                    onClicked: {
                                        accountsMenu.popup(parent)
                                    }
                                }
                            }
                        }
                    }
                }
            }

            RoundButton {
                visible: Utils.isAndroid()
                width: Utils.isAndroid() ? 64 : 48
                height: Utils.isAndroid() ? 64 : 48
                text: "+"
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 16
                anchors.right: parent.right
                anchors.rightMargin: 16
                font.pointSize: Utils.isAndroid() ? 24 : 14
                font.bold: true
                onClicked: {
                     stackPages.currentIndex = 1
                }
            }
        }

        Accounts.AddAccountForm {
            id: addAccountForm
            Layout.fillHeight: true
            Layout.fillWidth: true
            appId: "org.mauikit.test"
            onAccountAdded: {
                console.log("Account Secret :", secret)

                stackPages.currentIndex = 0
                Accounts.Controller.getAccountList()
            }

            RoundButton {
                width: Utils.isAndroid() ? 64 : 48
                height: Utils.isAndroid() ? 64 : 48
                icon.source: "icons/back.png"
                z: 100
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 16
                anchors.right: parent.right
                anchors.rightMargin: 16
                visible: stackPages.currentIndex > 0 && addAccountForm.swipeView.currentIndex == 0
                onClicked: {
                    stackPages.currentIndex = 0
                }
            }
        }
    }

    Connections {
        target: Accounts.Controller

        onMasterPasswordNotSet: {
            console.log("Password not set")
            passwordDialog.open()
        }

        onMasterPasswordSet: {
            console.log("Password already set")
            Accounts.Controller.getAccountList()
        }

        onAccountList: {
            listmodelAccounts.clear()

            for (var i=0; i<accounts.length; i++) {
                var account = accounts[i]

                listmodelAccounts.append({
                    accountName: account.username + " - " + account.url,
                    id: account.id,
                    appId: account.appId,
                    type: account.type,
                    username: account.username,
                    url: account.url,
                    host: account.host
                })
            }
        }

//        onShowIndefiniteProgress: {
//            progressDialogText.text = message
//            progressDialog.open()
//        }

//        onHideIndefiniteProgress: {
//            progressDialogText.text = ""
//            progressDialog.close()
//        }
    }

    Component.onCompleted: {
        Accounts.Controller.checkIfMasterPasswordSet()
    }
}




























































































































































































/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
