// dear imgui: standalone example application for Emscripten, using SDL2 + OpenGL3
// This is mostly the same code as the SDL2 + OpenGL3 example, simply with the modifications needed to run on Emscripten.
// It is possible to combine both code into a single source file that will compile properly on Desktop and using Emscripten.
// See https://github.com/ocornut/imgui/pull/2492 as an example on how to do just that.
//
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (Emscripten is a C++-to-javascript compiler, used to publish executables for the web. See https://emscripten.org/)

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <emscripten.h>
#include <string>
//#include <emscripten/bind.h>
#include <SDL.h>
#include <SDL_opengles2.h>

// Emscripten requires to have full control over the main loop. We're going to store our SDL book-keeping variables globally.
// Having a single function that acts as a loop prevents us to store state in the stack of said function. So we need some location for this.
SDL_Window*     g_Window = NULL;
SDL_GLContext   g_GLContext = NULL;

//using namespace emscripten;

int rmain(int, char**);

extern "C" {
void onerror(const char* message,const char* source){
	printf("ERROR %s %s\n",message,source);
}
int main(int i, char** c){
	EM_ASM(
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
	rmain(i,c);
}
}

bool first = true;
int frame = 0;
worker_handle thrift_worker;
unsigned int pongs = 0;
char echoAnswer[500];

void cback(char* data, int size, void* arg) {
    std::string type((char*)arg);
    printf("Callback: %s, %s\n", data, arg);
    
    if(type == "ping"){
		pongs++;
	}else if(type == "echo"){
		strcpy(echoAnswer, data);
	}
    
}

// For clarity, our main loop code is declared at the end.
void main_loop(void*);

int rmain(int, char**)
{

    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // For the browser using Emscripten, we are going to use WebGL1 with GL ES2. See the Makefile. for requirement details.
    // It is very likely the generated file won't work in many browsers. Firefox is the only sure bet, but I have successfully
    // run this code on Chrome for Android for example.
    const char* glsl_version = "#version 100";
    //const char* glsl_version = "#version 300 es";
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
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = NULL;

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(g_Window, g_GLContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // This function call won't return, and will engage in an infinite loop, processing events from the browser, and dispatching them.
    emscripten_set_main_loop_arg(main_loop, NULL, 0, true);
    return 0;
}



void main_loop(void* arg)
{
	//emscripten_sleep(1);
	frame ++;
	if(first){
		//emscripten_async_call((em_arg_callback_func)extraTest, nullptr, 1);
		thrift_worker = emscripten_create_worker("thrift-worker.js");
		std::string text = "open";
		std::string funcname = "tw_open";
		emscripten_call_worker(thrift_worker, funcname.c_str() , 0, 0, cback, (void*)text.c_str() );
		first = false;
	}
	
	if(frame%200 == 1){
		char data[50];
		std::string text = "call you";
		std::string funcname = "callback";
		
		//printf("Got response %s \n", data);
	}
	
    ImGuiIO& io = ImGui::GetIO();
    IM_UNUSED(arg); // We can pass this argument as the second parameter of emscripten_set_main_loop_arg(), but we don't use that.

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
		static char textfeld[500];

        ImGui::Begin("Emscripten Test");
        
        ImGui::Text("Ping Test");
        if (ImGui::Button("Ping")){
			std::string text = "ping";
			std::string funcname = "tw_ping";
			emscripten_call_worker(thrift_worker, funcname.c_str() , 0, 0, cback, (void*)text.c_str() );
		}
        ImGui::SameLine();
        ImGui::Text("counter = %d", pongs);
		
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		
		ImGui::Text("Echo Test");
		ImGui::InputTextMultiline("", textfeld, 500);
		if (ImGui::Button("Send")){
			char data[500];
			strcpy(data, textfeld);
			std::string text = "echo";
			std::string funcname = "tw_echo";
			emscripten_call_worker(thrift_worker, funcname.c_str() ,data , 500, cback, (void*)text.c_str() );
		}
		ImGui::Spacing();
		ImGui::Text("Answer:");
		ImGui::TextWrapped("%s", echoAnswer);
		
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
