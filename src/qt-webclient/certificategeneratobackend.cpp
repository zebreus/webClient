#include "certificategeneratobackend.h"

CertificateGeneratorBackend::CertificateGeneratorBackend(QObject *parent) : QObject(parent)
{
    m_hostName = "localhost";
    m_state = CLOSED;
    m_useWss = false;
    m_hostPort = 443;
    gc = new CertificateGeneratorConnection();
    connect(gc, &CertificateGeneratorConnection::connectionEstablished, this, &CertificateGeneratorBackend::connectionEstablished);
    connect(gc, &CertificateGeneratorConnection::connectionClosed, this, &CertificateGeneratorBackend::connectionClosed);
    connect(gc, &CertificateGeneratorConnection::connectionFailed, this, &CertificateGeneratorBackend::connectionFailed);
    connect(gc, &CertificateGeneratorConnection::documentsGenerated, this, &CertificateGeneratorBackend::documentsGenerated);
    initFs();
//    vector<string> strings;
//    strings.push_back("test1");
//    strings.push_back("test2");
//    strings.push_back("test3");
//    strings.push_back("test4, er ist seeeeeehr lang!!!!!!!!!!!");
//    strings.push_back("test5");
//    documentsGenerated(strings);
}

QString CertificateGeneratorBackend::hostName()
{
    return m_hostName;
}

int CertificateGeneratorBackend::hostPort()
{
    return m_hostPort;
}

bool CertificateGeneratorBackend::useWss()
{
    return m_useWss;
}

QString CertificateGeneratorBackend::getStateName()
{
    switch(m_state){
        case CLOSED:
            return "CLOSED";
        case OPENING:
            return "OPENING";
        case CLOSING:
            return "CLOSING";
        case READY:
            return "READY";
        case GENERATING:
            return "GENERATING";
        case ERROR:
            return "ERROR";
    }
}

void CertificateGeneratorBackend::setHostName(const QString &hostName)
{
    if (hostName == m_hostName){
       return;
    }
    m_hostName = hostName;
    emit hostNameChanged();
}

void CertificateGeneratorBackend::setHostPort(const int &hostPort)
{
    if(hostPort == m_hostPort){
        return;
    }
    m_hostPort = hostPort;
    emit hostPortChanged();
}

void CertificateGeneratorBackend::setUseWss(const bool &useWss)
{
    cout << "WSS = " << useWss << endl;
    if(useWss == m_useWss){
        return;
    }
    m_useWss = useWss;
    emit useWssChanged();
}

QString CertificateGeneratorBackend::getOpenButtonText()
{
    if(m_state == READY || m_state == GENERATING || m_state == CLOSING){
        return "Close";
    }else{
        return "Open";
    }
}

bool CertificateGeneratorBackend::getOpenButtonEnabled()
{
    if(m_state == OPENING || m_state == GENERATING || m_state == CLOSING){
        return false;
    }else{
        return true;
    }
}

bool CertificateGeneratorBackend::getGenerateButtonEnabled()
{
    if(m_state == READY){
        return true;
    }else{
        return false;
    }
}

QVariant CertificateGeneratorBackend::getFileList()
{
    return  QVariant::fromValue(m_fileList);
}

void CertificateGeneratorBackend::downloadFile(QString filename)
{
    cout << "Downloading:" << filename.toStdString() << endl;
    openPdf(filename.toStdString());
}

void CertificateGeneratorBackend::onOpenButtonClicked()
{
    cout << "Open button clicked" << endl;
    if(m_state == CLOSED || m_state == ERROR){
        m_state = OPENING;
        emit stateChanged();
        gc->openConnection(m_hostName.toStdString(), m_hostPort, m_useWss);
    }else{
        m_state = CLOSING;
        emit stateChanged();
        gc->closeConnection();
    }

}

void CertificateGeneratorBackend::onGenerateButtonClicked()
{
    cout << "Open generate clicked" << endl;
    if(m_state == READY){
        m_state = GENERATING;
        emit stateChanged();
        gc->generateDocuments("exampleJob.tex", exampleTemplate, exampleData);
    }
}

void CertificateGeneratorBackend::connectionEstablished()
{
    cout << "Connection established" << endl;
    m_state = READY;
    emit stateChanged();
}

void CertificateGeneratorBackend::connectionClosed()
{
    cout << "Connection closed" << endl;
    m_state = CLOSED;
    emit stateChanged();
}

void CertificateGeneratorBackend::connectionFailed()
{
    cout << "Connection failed" << endl;
    m_state = ERROR;
    emit stateChanged();
}

void CertificateGeneratorBackend::documentsGenerated(vector<string> filenames)
{
    cout << "Documents generated" << endl;
    m_fileList.clear();
    for(string s: filenames){
        cout << s << endl;
        m_fileList << QString::fromStdString(s);
    }
    m_state = READY;
    emit stateChanged();
    emit fileListChanged();
}
