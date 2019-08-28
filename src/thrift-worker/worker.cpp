#include <worker.hpp>

void tw_open(char* data, int size){
	//Get address
	string address(data);
	
	if(address.find("wss://") == 0){
		address = address.substr(6, string::npos);
		EM_ASM(
			Module['websocket'].url='wss://';
			Module['websocket'].subprotocol='binary';
		);
	}else if(address.find("ws://") == 0){
		address = address.substr(5, string::npos);
		EM_ASM(
			Module['websocket'].url='ws://';
			Module['websocket'].subprotocol='binary';
		);
	}
	
	
	int seperator = address.find_last_of(":");
	if(seperator == string::npos){
		//TODO find right thing to throw
		throw 0;
	}
	int port = stoi(address.substr(seperator+1 , string::npos) );
	address = address.substr(0, seperator);
	
	//Initialize connection
	TSocket* socket_ = new TSocket(address, port);
	std::shared_ptr<TTransport> socket(socket_);
	std::shared_ptr<TTransport> ___transport(new TBufferedTransport(socket));
	__transport = ___transport;
	std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(__transport));
	std::shared_ptr<CertificateGeneratorClient> ___client(new CertificateGeneratorClient(protocol));
	__client = ___client;
	
	//Open connection
	__transport->open();
	emscripten_worker_respond(nullptr, 0);
}

void tw_close(char* data, int size){
	__transport->close();
	emscripten_worker_respond(nullptr, 0);
}

void tw_generateCertificates(char* data, int size){
	vector<GeneratedFile> result;
	__client->generateCertificates( result, example_data );
	//Write files to IDBFS
	for(GeneratedFile file : result){
		string filename = "/generated/";
		filename.append(file.name);
		ofstream fileout;
		fileout.open(filename.c_str(), std::ios::out | std::ios::binary);
		fileout << file.content << endl ;
		fileout.close();
		EM_ASM(
			FS.syncfs(function (err) {
			});
		);
		//Respond with path
		char* response = new char[filename.size()+1];
		memcpy(response, filename.c_str(), filename.size()+1);
		emscripten_worker_respond_provisionally(response,filename.size()+1);
		delete[] response;
	}
	//Send empty response, when finished
	emscripten_worker_respond(0,0);
}

int main(int argc, char** argv){
	//Mount IDBFS
	EM_ASM(
		FS.mkdir('/generated');
		FS.mount(IDBFS, {}, '/generated');
		FS.syncfs(true, function (err) {
		});
	);
}
