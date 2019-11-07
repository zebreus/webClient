#ifndef CERTIFICATEGENERATORCONNECTION_H
#define CERTIFICATEGENERATORCONNECTION_H

#include <QObject>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

class CertificateGeneratorConnectionInterface : public QObject
{
Q_OBJECT public:
    explicit CertificateGeneratorConnectionInterface(QObject *parent = nullptr) : QObject(parent){}
    virtual void openConnection(string hostname, int port, bool wss) = 0;
    virtual void closeConnection() = 0;
    virtual void generateDocuments(string name, string templateFile, string data) = 0;
public:
signals:
    void connectionEstablished();
    void connectionClosed();
    void connectionFailed();
    void documentsGenerated(vector<string>);
};


#ifdef __EMSCRIPTEN__
    //#include "emscripten/certificategeneratorconnection.h"

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


#else
    //#include "linux/certificategeneratorconnection.h"

#include <gen-cpp/CertificateGenerator.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace ::CertificateGeneratorThrift;

using namespace std;

class CertificateGeneratorConnection : public CertificateGeneratorConnectionInterface
{
public:
    explicit CertificateGeneratorConnection(QObject *parent = nullptr);
    void openConnection(string hostname, int port, bool wss);
    void closeConnection();
    void generateDocuments(string name, string templateFile, string data);
private:
    std::shared_ptr<TTransport> m_transport;
    std::shared_ptr<CertificateGeneratorClient> m_client;
};

#endif

#endif // CERTIFICATEGENERATORCONNECTION_H
