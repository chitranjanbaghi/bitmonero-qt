import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.1

import "qrc:/qml/Utils"

AbstractPage {

    signal walletFilenameSet(var filename);
    signal walletPasswordSet(var password)

    stack: myStack
    nextPage: confirmNewWalletPage

    ColumnLayout {

        anchors.horizontalCenter: parent.horizontalCenter

        ColumnLayout {

            Label {
                id: newWalletNameLabel

                text: "Choose the name of your wallet"
            }

            TextField {
                id: newWalletNameInput

                placeholderText: "Name of your Wallet"
                textColor: if (acceptableInput) { "green" } else { "red" }
                validator: RegExpValidator { regExp: /[a-zA-Z0-9]+/ }

            }
        }


        ColumnLayout {
            Label {
                id: newWalletPasswordLabel

                text: "Choose password for your wallet (4 chars min)"
            }

            PasswordTextField {
                id: newWalletPasswordInput

                placeholderText: "Password for your Wallet"

            }

        }

        ColumnLayout {

            CheckBox {
                id: useDefaultLocationCheckbox

                text: "Use default Wallet location"
                checked: true
            }

            TextField {
                id: newWalletLocationInput

                text: wallet_handler.default_wallet_location;
                visible: !useDefaultLocationCheckbox.checked
            }

        }

        Button {
            text: "Create Wallet"
            onClicked: if (newWalletNameInput.acceptableInput && newWalletPasswordInput.acceptableInput) {

                           walletPasswordSet(newWalletPasswordInput.text)
                           walletFilenameSet(newWalletLocationInput.text + "/" + newWalletNameInput.text)

                           goToNext();

                       }

        }

    }

}




