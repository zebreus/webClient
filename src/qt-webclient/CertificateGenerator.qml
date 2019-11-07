import QtQuick 2.13
import QtQuick.Controls 2.13
import webclient.certificategeneratorbackend 1.0

CertificateGeneratorForm {
        CertificateGeneratorBackend{
            id: backend
        }

        openButton{
            text: backend.openButtonText
            enabled: backend.openButtonEnabled
            onClicked: backend.onOpenButtonClicked()
        }
        generateButton{
            enabled: backend.generateButtonEnabled
            onClicked: backend.onGenerateButtonClicked()
        }
        hostname{
            onTextChanged: backend.hostName = hostname.text
            text: backend.hostname
        }
        spinBox{
            value: backend.hostPort
            onValueChanged: backend.hostPort = spinBox.value
        }
        wssBox{
            checked: backend.useWss
            onCheckedChanged: backend.useWss = wssBox.checked
        }
        stateLabel{
            text: backend.stateName
        }
        fileListView{
            model: backend.fileList
            delegate: FileListItem {
                text: modelData
                Connections {
                    target: itemButton
                    onClicked: backend.downloadFile(modelData)
                }
            }
        }
}
