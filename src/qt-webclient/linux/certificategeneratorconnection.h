#ifndef CERTIFICATEGENERATORCONNECTION_H_IMPL
#define CERTIFICATEGENERATORCONNECTION_H_IMPL

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
