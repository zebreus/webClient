#include <certificategeneratorconnection.h>

CertificateGeneratorConnection::CertificateGeneratorConnection(QObject *parent) : CertificateGeneratorConnectionInterface(parent)
{
}

void CertificateGeneratorConnection::openConnection(string hostname, int port, bool wss)
{
    cout << "Doing emscripten stuff" << endl;
    thriftWorker = emscripten_create_worker("thrift-worker.js");
    string funcname = "tw_open";
    string addressString;
    if(wss){
        addressString.append("wss://");
    }else{
        addressString.append("ws://");
    }
    addressString.append( string(hostname) );
    addressString.append(":");
    addressString.append(to_string(port));
    char* address = new char[addressString.size()+1];
    memcpy(address, addressString.c_str(), addressString.size()+1);
    emscripten_call_worker(thriftWorker, funcname.c_str() , address, addressString.size()+1, CertificateGeneratorConnection::connectionEstablishedWorkerCallback, (void*) this);
    delete[] address;
    cout << "Did emscripten stuff" << endl;
}

void CertificateGeneratorConnection::closeConnection()
{
    std::string funcname = "tw_close";
    emscripten_call_worker(thriftWorker, funcname.c_str() , 0, 0, CertificateGeneratorConnection::connectionClosedWorkerCallback, (void*) this);
}

void CertificateGeneratorConnection::generateDocuments(string name, string documentTemplate, string documentData)
{
    std::string funcname = "tw_generateCertificates";
    emscripten_call_worker(thriftWorker, funcname.c_str() , 0, 0, CertificateGeneratorConnection::documentsGeneratedWorkerCallback, (void*) this);
}

void CertificateGeneratorConnection::connectionEstablishedWorkerCallback(char* data, int size, void* arg){
    emit ((CertificateGeneratorConnection*)arg)->connectionEstablished();
}
void CertificateGeneratorConnection::connectionClosedWorkerCallback(char* data, int size, void* arg){
    emscripten_destroy_worker(((CertificateGeneratorConnection*)arg)->thriftWorker);
    emit ((CertificateGeneratorConnection*)arg)->connectionClosed();
}
void CertificateGeneratorConnection::documentsGeneratedWorkerCallback(char* data, int size, void* arg){
    if(size != 0){
        ((CertificateGeneratorConnection*)arg)->files.push_back(string(data));
    }else{
        emit ((CertificateGeneratorConnection*)arg)->documentsGenerated(((CertificateGeneratorConnection*)arg)->files);
    }
}
