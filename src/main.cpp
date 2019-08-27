#include "main.hpp"

void thriftOpenCallback(char* data, int size, void* arg){
	printf("OpenCallback: %s, %s\n", data, arg);
	connectionState = READY;
}

void thriftCloseCallback(char* data, int size, void* arg){
	printf("CloseCallback: %s, %s\n", data, arg);
	emscripten_destroy_worker(thriftWorker);
	connectionState = CLOSED;
}

void thriftGenerateCallback(char* data, int size, void* arg){
	printf("GenerateCallback: %s, %s\n", data, arg);
	if(size != 0){
		files.push_back(string(data));
	}else{
		connectionState = READY;
	}
}

void onerror(const char* message,const char* source){
	printf("ERROR %s %s\n",message,source);
	string sourceString(source);
	if(sourceString.find(THRIFT_WORKER_FILE)){
		emscripten_destroy_worker(thriftWorker);
		connectionState = ERROR;
	}
}

int main(int, char**)
{
	EM_ASM(
		// Set error callback, to direct errors to our onerror method
		window.onerror = function(message, source, lineno, colno, error){
			var messageLen = lengthBytesUTF8(message);
			var messagePtr = _malloc(messageLen+1);
			stringToUTF8(message, messagePtr, messageLen+1);
			var sourceLen = lengthBytesUTF8(source);
			var sourcePtr = _malloc(sourceLen+1);
			stringToUTF8(source, sourcePtr, sourceLen+1);
			
			_onerror(messagePtr, sourcePtr);
			
			_free(messagePtr);
			_free(sourcePtr);
			
			return false;
		}
	);
	EM_ASM(
		// Mount IndexDBFS to /generated. This is, where the worker
		// will place the generated files
		FS.mkdir('/generated');
		FS.mount(IDBFS, {}, '/generated');
		FS.syncfs(true, function (err) {
		});
	);
	
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // For the browser using Emscripten, we are going to use WebGL1 with GL ES2.
    // It is very likely the generated file won't work in many browsers. Firefox is the only sure bet, but I have successfully
    // run this code on Chrome for Android for example.
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    g_Window = SDL_CreateWindow("Dear ImGui Emscripten example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    g_GLContext = SDL_GL_CreateContext(g_Window);
    if (!g_GLContext)
    {
        fprintf(stderr, "Failed to initialize WebGL context!\n");
        return 1;
    }
    SDL_GL_SetSwapInterval(1);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = NULL;

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(g_Window, g_GLContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Start the main loop
    emscripten_set_main_loop_arg(main_loop, NULL, 0, true);
    return 0;
}



void main_loop(void* arg)
{
	
    ImGuiIO& io = ImGui::GetIO();
    IM_UNUSED(arg);

    // Our state (make them static = more or less global) as a convenience to keep the example terse.
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Poll and handle events (inputs, window resize, etc.)
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(g_Window);
    ImGui::NewFrame();

	{
		static char hostname[100] = "localhost";
		static int hostport = 9090;
		
		//Open new window
        ImGui::Begin("Certificate Generator", NULL, ImGuiWindowFlags_AlwaysAutoResize);
        
        // Display state
        string state;
        switch(connectionState){
			case CLOSED:
				state = "CLOSED";
				break;
			case OPENING:
				state = "OPENING";
				break;
			case CLOSING:
				state = "CLOSING";
				break;
			case READY:
				state = "READY";
				break;
			case GENERATING:
				state = "GENERATING";
				break;
			case ERROR:
				state = "ERROR";
				break;
		}
		ImGui::Text("STATE: %s", state.c_str());
		
		//Read Servername and port
		ImGui::InputText("Hostname", hostname, 100);
		ImGui::InputInt("Port", &hostport);
		
		//Button to open/close connection
		if(connectionState == CLOSED || connectionState == ERROR){
			if (ImGui::Button("Open")){
				thriftWorker = emscripten_create_worker(THRIFT_WORKER_FILE);
				std::string funcname = "tw_open";
				std::string addressString(hostname);
				addressString.append(":");
				addressString.append(to_string(hostport));
				char* address = new char[addressString.size()+1];
				memcpy(address, addressString.c_str(), addressString.size()+1);
				emscripten_call_worker(thriftWorker, funcname.c_str() , address, addressString.size()+1, thriftOpenCallback, 0);
				delete[] address;
				connectionState = OPENING;
			}
		}else if(connectionState == READY){
			if (ImGui::Button("Close")){
				std::string funcname = "tw_close";
				emscripten_call_worker(thriftWorker, funcname.c_str() , 0, 0, thriftCloseCallback, 0);
				connectionState = CLOSING;
			}
		}else{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			ImGui::Button("Open");
			ImGui::PopStyleVar();
		}
		
		//Button to generate certificates
		if(connectionState == READY){
			if (ImGui::Button("Generate certificates")){
				if(connectionState == READY){
					std::string funcname = "tw_generateCertificates";
					files.clear();
					emscripten_call_worker(thriftWorker, funcname.c_str() , 0, 0, thriftGenerateCallback, 0);
					connectionState = GENERATING;
				}
			}
		}
		if(connectionState != READY){
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			ImGui::Button("Generate certificates");
			ImGui::PopStyleVar();
		}

		
		// Download generated files
		ImGui::Text("Generated certificates:");
		for(string filename : files){
			if (ImGui::Button(filename.c_str())){
				char* cFilename = new char[filename.size()+1];
				std::memcpy(cFilename, filename.c_str(), filename.size()+1);
				downloadFile(cFilename,"application/pdf");
				delete[] cFilename;
			}
		}
		
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
	}

    // Rendering
    ImGui::Render();
    SDL_GL_MakeCurrent(g_Window, g_GLContext);
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(g_Window);
}
