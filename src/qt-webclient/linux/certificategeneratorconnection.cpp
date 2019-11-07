#include <certificategeneratorconnection.h>

CertificateGeneratorConnection::CertificateGeneratorConnection(QObject *parent) : CertificateGeneratorConnectionInterface(parent)
{
}

void CertificateGeneratorConnection::openConnection(string hostname, int port, bool wss)
{
    std::shared_ptr<TTransport> socket(new TSocket(hostname, port));
    std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    m_transport = transport;
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(m_transport));
    std::shared_ptr<CertificateGeneratorClient> client(new CertificateGeneratorClient(protocol));
    m_client = client;
    try {
        transport->open();
        emit connectionEstablished();
      } catch (TException& tx) {
        emit connectionFailed();
    }
}

void CertificateGeneratorConnection::closeConnection()
{
    if( m_transport->isOpen() ){
        try {
            m_transport->close();
            emit connectionClosed();
        }  catch (TException& tx) {
            emit connectionFailed();
        }
    }
}

void CertificateGeneratorConnection::generateDocuments(string name, string documentTemplate, string documentData)
{
    //Set data
    File templateFile;
    templateFile.name = "name";
    templateFile.content = documentTemplate;
    m_client->addTemplateFile(templateFile);
    m_client->setConfigurationData(documentData);

    if( m_transport->isOpen() ){
        vector<File> result;
        vector<string> filenames;
        m_client->generateCertificates( result );
        for(File file : result ){
            cout << "generated:" << file.name << endl;
            string filename = "generated/";
            filename.append(file.name);
            ofstream fileout;
            fileout.open(filename.c_str(), std::ios::out | std::ios::binary);
            fileout << file.content << endl ;
            fileout.close();
            filenames.push_back(filename);
        }
        emit documentsGenerated(filenames);
    }
}
