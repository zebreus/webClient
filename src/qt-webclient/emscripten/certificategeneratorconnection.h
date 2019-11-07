#ifndef CERTIFICATEGENERATORCONNECTION_H
    #include "certificategeneratorconnection.h"
#else

#include <QObject>
#include <emscripten.h>

using namespace std;

class CertificateGeneratorConnection : public CertificateGeneratorConnectionInterface
{
Q_OBJECT public:
    explicit CertificateGeneratorConnection(QObject *parent = nullptr);
    void openConnection(string hostname, int port, bool wss);
    void closeConnection();
    void generateDocuments(string name, string templateFile, string data);
private:
    vector<string> files;
    worker_handle thriftWorker;
    static void connectionEstablishedWorkerCallback(char* data, int size, void* arg);
    static void connectionClosedWorkerCallback(char* data, int size, void* arg);
    static void documentsGeneratedWorkerCallback(char* data, int size, void* arg);
public:
signals:
    void connectionEstablished();
    void connectionClosed();
    void connectionFailed();
    void documentsGenerated(vector<string>);
};

#endif
