#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <emscripten.h>
#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_opengles2.h>

using namespace std;

enum state{ CLOSED, OPENING, CLOSING, READY, GENERATING, ERROR };

/** @brief Offers a file from IDBFS for download
* @param [in] filename a char* containing the path of the file on IDBFS
* @param [in] mimetype a char* containing the mimetype of the file
* 
* Loads the file with filename from IDBFS and offers it for download
*/
EM_JS(void, downloadFile, (const char* filename, const char* mimetype), {
	let name = UTF8ToString(filename);
	let type = UTF8ToString(mimetype);
	FS.syncfs(true, function (err) {
		let content = FS.readFile(name,  { encoding: 'binary' });
		let a = document.createElement('a');
		var pos = name.lastIndexOf("/", name.length);
		a.download = name.slice(pos+1,name.length);
		a.href = URL.createObjectURL(new Blob([content], {type: type}));
		a.click();
		setTimeout(() => {
			URL.revokeObjectURL(a.href);
		}, 60000);
	});
});

extern "C" {
	/** @brief Handles errors
    * @param [in] message a char* containing the error message
    * @param [in] source a char* containing the file in which the error was triggered
    * 
    * Destroys the thriftworker and sets the connection state to ERROR if
    * the error came from thrift_worker.js
    */
	void onerror(const char* message,const char* source);
	
	/** @brief Initializes the website
    * @param [in] argc a int specifying the number of arguments in argv
    * @param [in] argv a char** representing the arguments
    * @return A int containing the exit status
    * 
    * Sets up error handling, to trigger onerror
    * Mounts /generated from IDBFS
    * Sets up ImGui
    * Sets the mainloop
    */
	int main(int argc, char** argv);
}

/** @brief The main loop of the program
* 
* Draws the windows and reacts to interactions
*/
void main_loop(void*);

/** @brief Sets the connection state to OPEN.
* @param [in] data a char*
* @param [in] size a int
* @param [in] arg a void*
* 
* A callback for when a connection to the thrift server is open.
* Sets the connection state to OPEN.
*/
void thriftOpenCallback(char* data, int size, void* arg);

/** @brief Sets the connection state to CLOSED.
* @param [in] data a char*
* @param [in] size a int
* @param [in] arg a void*
* 
* A callback for when a connection to the thrift server is closed.
* Sets the connection state to CLOSED and destroys the thriftWorker.
*/
void thriftCloseCallback(char* data, int size, void* arg);

/** @brief Receives the generated files.
* @param [in] data a char* containing a path on IndexedDBFS
* @param [in] size a int containing the length of the path
* @param [in] arg a void*
* 
* A callback for when a file was generated.
* When a file is returned via data, it will be added to files.
* When all files are received, the connection state is set to READY.
*/
void thriftGenerateCallback(char* data, int size, void* arg);

SDL_Window*     g_Window = nullptr;
SDL_GLContext   g_GLContext = nullptr;
worker_handle thriftWorker;
state connectionState = CLOSED;
vector<string> files;

