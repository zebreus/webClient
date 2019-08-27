#include <string>
#include <fstream>
#include <gen-cpp/CertificateGenerator.h>
#include <emscripten.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace ::CertificateGeneratorThrift;
using namespace std;

std::shared_ptr<TTransport> __transport;
std::shared_ptr<CertificateGeneratorClient> __client;
string example_data = "{\
\"students\":[\
	{\
	\"name\":\"Max\",\
	\"surname\":\"Mustermann\",\
	\"date\":\"14. August 1998\",\
	\"tasks\":[\
		{\
		\"name\":\"showing up\",\
		\"grade\":\"2.3\"\
		},\
		{\
		\"name\":\"breathing\",\
		\"grade\":\"2.7\"\
		}\
	]\
	},\
	{\
	\"name\":\"Tim\",\
	\"surname\":\"Testperson\",\
	\"date\":\"8. April 2019\",\
	\"tasks\":[\
		{\
		\"name\":\"showing up\",\
		\"grade\":\"1.0\"\
		},\
		{\
		\"name\":\"not dying\",\
		\"grade\":\"1.0\"\
		},\
		{\
		\"name\":\"breathing\",\
		\"grade\":\"1.0\"\
		}\
	]\
	}\
],\
\"tester\":\"Someone Important\"\
}\
";

extern "C"
{
	/** @brief Opens the connection to the thrift server
    * @param [in] data a char* containing the server address
    * @param [in] size a int containing the length of the server address
    * 
    * Opens the connection to the thrift server.
    * data contains the address formatted like HOSTNAME:PORT
    */
	void tw_open(char* data, int size);
	
	/** @brief Closes the connection to the thrift server
    * @param [in] data a char*
    * @param [in] size a int
    * 
    * Closes the connection to the thrift server.
    * Before using this function you have to connect to a server.
    */
	void tw_close(char* data, int size);
	
	/** @brief Generates pdf files on the server
    * @param [in] data a char*
    * @param [in] size a int
    * 
    * Generates pdf files on the server, stores them in IndexedDBFS and
    * returns their paths via emscripten_worker_respond_provisionally.
    * If all files are returned emscripten_worker_respond is called
    * without a return value.
    * Before using this function you have to connect to a server.
    */
	void tw_generateCertificates(char* data, int size);
	
	/** @brief Initializes the worker
    * @param [in] argc a int specifying the number of arguments in argv
    * @param [in] argv a char** representing the arguments
    * @return A int containing the exit status
    * 
    * Initializes the filesystem for the worker
    */
	int main(int argc, char** argv);
}
