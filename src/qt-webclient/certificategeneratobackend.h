#ifndef CERTIFICATEGENERATOBACKEND_H
#define CERTIFICATEGENERATOBACKEND_H

#include "certificategeneratorconnection.h"
#include "supportfunctions.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QTimer>
#include <iostream>
#include <string>
#include <vector>
#include <exampledata.h>
using namespace std;

enum state{ CLOSED, OPENING, CLOSING, READY, GENERATING, ERROR };

class CertificateGeneratorBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString stateName READ getStateName NOTIFY stateChanged)
    Q_PROPERTY(QString openButtonText READ getOpenButtonText NOTIFY stateChanged)
    Q_PROPERTY(bool openButtonEnabled READ getOpenButtonEnabled NOTIFY stateChanged)
    Q_PROPERTY(bool generateButtonEnabled READ getGenerateButtonEnabled NOTIFY stateChanged)
    Q_PROPERTY(bool useWss READ useWss WRITE setUseWss NOTIFY useWssChanged)
    Q_PROPERTY(QString hostName READ hostName WRITE setHostName NOTIFY hostNameChanged)
    Q_PROPERTY(int hostPort READ hostPort WRITE setHostPort NOTIFY hostPortChanged)
    Q_PROPERTY(QVariant fileList READ getFileList NOTIFY fileListChanged)
public:
    explicit CertificateGeneratorBackend(QObject *parent = nullptr);
    QString hostName();
    int hostPort();
    bool useWss();
    QString getStateName();
    void setHostName(const QString& hostName);
    void setHostPort(const int& hostPort);
    void setUseWss(const bool& useWss);
    QString getOpenButtonText();
    bool getOpenButtonEnabled();
    bool getGenerateButtonEnabled();
    QVariant getFileList();
private:
    QStringList m_fileList;
    QString m_hostName;
    int m_hostPort;
    bool m_useWss;
    state m_state;
    CertificateGeneratorConnection* gc = nullptr;

signals:
    void hostNameChanged();
    void useWssChanged();
    void hostPortChanged();
    void stateChanged();
    void fileListChanged();

public slots:
    void downloadFile(QString filename);
    void onOpenButtonClicked();
    void onGenerateButtonClicked();
    void connectionEstablished();
    void connectionClosed();
    void connectionFailed();
    void documentsGenerated(vector<string>);
};

#endif // CERTIFICATEGENERATOBACKEND_H
