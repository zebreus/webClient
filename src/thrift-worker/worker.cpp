#include <stdio.h>
#include <emscripten.h>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>

#include <gen-cpp/Example.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace example;

using namespace std;

void main_cpp(int, char**);
void open_cpp();
void close_cpp();
void ping_cpp();
void echo_cpp(char* text, int size);

std::shared_ptr<TTransport> __transport;
std::shared_ptr<ExampleClient> __client;

extern "C"
{
int finished = 0;

void callback(char* data, int size){
	char working[8] = "WORKING";
	char finishedText[9] = "FINISHED";
	if(finished == 0){
		emscripten_worker_respond(working , 8);
	}else{
		emscripten_worker_respond(finishedText ,9);
	}
}

void tw_open(char* data, int size){
	//~ TSocket* socket_ = new TSocket("127.0.0.1", 9090);
	//~ std::shared_ptr<TTransport> socket(socket_);
	//~ std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	//~ std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	//~ ExampleClient client(protocol);
	TSocket* socket_ = new TSocket("127.0.0.1", 9090);
	std::shared_ptr<TTransport> socket(socket_);
	std::shared_ptr<TTransport> ___transport(new TBufferedTransport(socket));
	__transport = ___transport;
	std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(__transport));
	std::shared_ptr<ExampleClient> ___client(new ExampleClient(protocol));
	__client = ___client;
	__transport->open();
	emscripten_worker_respond("e", 2);
}


void tw_close(char* data, int size){
	__transport->close();
	emscripten_worker_respond(nullptr, 0);
}

void tw_ping(char* data, int size){
	__client->ping();
	emscripten_worker_respond(nullptr, 0);
}

void tw_echo(char* data, int size){
	string input(data);
	string result;
	__client->echo( result, input );
	char* echoResult = new char[result.size()+1];
	memcpy( echoResult, result.c_str(), result.size()+1 );
	emscripten_worker_respond( echoResult, result.size()+1);
	//TODO free
}

int main(int i, char** c){
	main_cpp(i, c);
}
}

void main_cpp(int, char**)
{
}

void open_cpp(){
}
void close_cpp(){
}
void ping_cpp(){
}
void echo_cpp(char* text, int size){
}

