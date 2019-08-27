#
# Makefile to use with emscripten
# See https://emscripten.org/docs/getting_started/downloads.html
# for installation instructions. This Makefile assumes you have
# loaded emscripten's environment.
#
# Running `make` will produce three files:
#  - example_emscripten.html
#  - example_emscripten.js
#  - example_emscripten.wasm
#
# All three are needed to run the demo.

#TODO move boost into project or smth better
#TODO reduce .o targets
#TODO sort thrift
#TODO targets for thrift gen-cpp

BROWSER = chrome

#paths
OUTPUT = ./out/
MAIN = ./src/
EXAMPLE_WORKER = ./src/example-worker/
THRIFT_WORKER = ./src/thrift-worker/
THRIFT_GENERATED = $(THRIFT_WORKER)/gen-cpp/
IMGUI_GIT = ./libs/imgui/
THRIFT = ./libs/thrift/
EMS_SHELL = ./res/shell_minimal.html
THRIFT_FILE = ./res/CertificateGenerator.thrift
#TODO schoener
DEBUG = --emrun
RELEASE =
MODE = $(DEBUG)

#compiler
CC = emcc
CXX = em++ $(MODE)

EXAMPLE_WORKER_EXE = worker.js
EXAMPLE_WORKER_SOURCES = $(EXAMPLE_WORKER)/worker.cpp
#EXAMPLE_WORKER_OBJS = $(addsuffix .o, $(basename $(notdir $(EXAMPLE_WORKER_SOURCES))))
EXAMPLE_WORKER_OBJS = $(addsuffix .o, $(basename $(EXAMPLE_WORKER_SOURCES)))
EXAMPLE_WORKER_EMS = -s WASM=1
EXAMPLE_WORKER_EMS += -s ALLOW_MEMORY_GROWTH=0 -s BINARYEN_TRAP_MODE=clamp
EXAMPLE_WORKER_EMS += -s DISABLE_EXCEPTION_CATCHING=1 -s NO_EXIT_RUNTIME=0
EXAMPLE_WORKER_EMS += -s ASSERTIONS=1 -s NO_FILESYSTEM=1
EXAMPLE_WORKER_EMS += -s ASYNCIFY=1 -s BUILD_AS_WORKER=1 -s EXPORTED_FUNCTIONS='["_callback", "_main"]'
#-s 'EXTRA_EXPORTED_RUNTIME_METHODS=["ccall", "cwrap"]'  -s 'EXTRA_EXPORTED_RUNTIME_METHODS=["ccall", "cwrap", "stringToUTF8"]'
EXAMPLE_WORKER_CPP = -Wall -Wformat -Os -std=c++17 $(EXAMPLE_WORKER_EMS)
EXAMPLE_WORKER_LDFLAGS = $(EXAMPLE_WORKER_EMS)

THRIFT_WORKER_EXE = thrift-worker.js
THRIFT_WORKER_SOURCES = $(THRIFT_WORKER)/worker.cpp
THRIFT_WORKER_SOURCES += $(THRIFT_GENERATED)/CertificateGenerator.cpp
THRIFT_WORKER_SOURCES += $(THRIFT_GENERATED)/CertificateGenerator_types.cpp
THRIFT_WORKER_SOURCES += $(THRIFT_GENERATED)/CertificateGenerator_constants.cpp
THRIFT_WORKER_SOURCES += $(THRIFT)/TApplicationException.cpp \
                       $(THRIFT)/TOutput.cpp \
                       $(THRIFT)/VirtualProfiling.cpp \
                       $(THRIFT)/async/TAsyncChannel.cpp \
                       $(THRIFT)/async/TAsyncProtocolProcessor.cpp \
                       $(THRIFT)/async/TConcurrentClientSyncInfo.cpp \
                       $(THRIFT)/concurrency/ThreadManager.cpp \
                       $(THRIFT)/concurrency/TimerManager.cpp \
                       $(THRIFT)/processor/PeekProcessor.cpp \
                       $(THRIFT)/protocol/TDebugProtocol.cpp \
                       $(THRIFT)/protocol/TJSONProtocol.cpp \
                       $(THRIFT)/protocol/TBase64Utils.cpp \
                       $(THRIFT)/protocol/TMultiplexedProtocol.cpp \
                       $(THRIFT)/protocol/TProtocol.cpp \
                       $(THRIFT)/transport/TTransportException.cpp \
                       $(THRIFT)/transport/TFDTransport.cpp \
                       $(THRIFT)/transport/TFileTransport.cpp \
                       $(THRIFT)/transport/TSimpleFileTransport.cpp \
                       $(THRIFT)/transport/THttpTransport.cpp \
                       $(THRIFT)/transport/THttpClient.cpp \
                       $(THRIFT)/transport/THttpServer.cpp \
                       $(THRIFT)/transport/TSocket.cpp \
                       $(THRIFT)/transport/TPipe.cpp \
                       $(THRIFT)/transport/TPipeServer.cpp \
                       $(THRIFT)/transport/TSocketPool.cpp \
                       $(THRIFT)/transport/TServerSocket.cpp \
                       $(THRIFT)/transport/TNonblockingServerSocket.cpp \
                       $(THRIFT)/transport/TTransportUtils.cpp \
                       $(THRIFT)/transport/TBufferTransports.cpp \
                       $(THRIFT)/server/TConnectedClient.cpp \
                       $(THRIFT)/server/TServer.cpp \
                       $(THRIFT)/server/TServerFramework.cpp \
                       $(THRIFT)/server/TSimpleServer.cpp \
                       $(THRIFT)/server/TThreadPoolServer.cpp \
                       $(THRIFT)/server/TThreadedServer.cpp
#THRIFT_WORKER_OBJS = $(addsuffix .o, $(basename $(notdir $(THRIFT_WORKER_SOURCES))))
THRIFT_WORKER_OBJS = $(addsuffix .o, $(basename $(THRIFT_WORKER_SOURCES)))
THRIFT_WORKER_EMS = -s WASM=1
THRIFT_WORKER_EMS += -s ALLOW_MEMORY_GROWTH=0
THRIFT_WORKER_EMS += -s DISABLE_EXCEPTION_CATCHING=1 -s NO_EXIT_RUNTIME=1
THRIFT_WORKER_EMS += -s ASSERTIONS=1 -s NO_FILESYSTEM=0
THRIFT_WORKER_EMS += -s ASYNCIFY=1 -s BUILD_AS_WORKER=1 
THRIFT_WORKER_EMS += -s EXTRA_EXPORTED_RUNTIME_METHODS=FS
THRIFT_WORKER_EMS += -s EXPORTED_FUNCTIONS='["_tw_open", "_tw_close", "_tw_generateCertificates", "_main"]'
#-s 'EXTRA_EXPORTED_RUNTIME_METHODS=["ccall", "cwrap"]'  -s 'EXTRA_EXPORTED_RUNTIME_METHODS=["ccall", "cwrap", "stringToUTF8"]'
THRIFT_WORKER_CPP = -Wall -Wformat -Os -std=c++17
THRIFT_WORKER_CPP += -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H
THRIFT_WORKER_CPP += -I$(THRIFT_WORKER) -I$(THRIFT_GENERATED) -I$(THRIFT)/../ -I/home/creator/Downloads/boost_1_70_0
THRIFT_WORKER_CPP += $(THRIFT_WORKER_EMS)
THRIFT_WORKER_LDFLAGS = -L/home/creator/Downloads/boost_1_70_0/stage/lib $(THRIFT_WORKER_EMS)

MAIN_EXE = example_emscripten.html
MAIN_SOURCES = $(MAIN)/main.cpp
MAIN_SOURCES += $(IMGUI_GIT)/examples/imgui_impl_sdl.cpp $(IMGUI_GIT)/examples/imgui_impl_opengl3.cpp
MAIN_SOURCES += $(IMGUI_GIT)/imgui.cpp $(IMGUI_GIT)/imgui_demo.cpp $(IMGUI_GIT)/imgui_draw.cpp $(IMGUI_GIT)/imgui_widgets.cpp
MAIN_OBJS = $(addsuffix .o, $(basename $(MAIN_SOURCES)))
#MAIN_OBJS = $(addsuffix .o, $(basename $(notdir $(MAIN_SOURCES))))
UNAME_S := $(shell uname -s)

MAIN_EMS = --bind -s USE_SDL=2 -s WASM=1
MAIN_EMS += -s ALLOW_MEMORY_GROWTH=0 -s BINARYEN_TRAP_MODE=clamp
MAIN_EMS += -s DISABLE_EXCEPTION_CATCHING=1 -s NO_EXIT_RUNTIME=0
MAIN_EMS += -s ASSERTIONS=1 -s NO_FILESYSTEM=0
MAIN_EMS += -s ASYNCIFY=1 -s EXPORTED_FUNCTIONS='["_onerror", "_main"]'
MAIN_EMS += -s EXTRA_EXPORTED_RUNTIME_METHODS=FS
#MAIN_EMS += -s SAFE_HEAP=1    ## Adds overhead

MAIN_CPP = -I$(IMGUI_GIT)/examples/ -I$(IMGUI_GIT)
MAIN_CPP += -Wall -Wformat -Os -std=c++17
MAIN_CPP += -DTHRIFT_WORKER_FILE=\"$(THRIFT_WORKER_EXE)\"
MAIN_CPP += $(MAIN_EMS) 
MAIN_LDFLAGS = $(MAIN_EMS) --shell-file $(EMS_SHELL)

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

$(MAIN)/%.o:$(MAIN)/%.cpp
	echo OJS: $(MAIN_OBJS) ENDS
	$(CXX) $(MAIN_CPP) $(CXXFLAGS) -c -o $@ $<

$(IMGUI_GIT)/examples/%.o:$(IMGUI_GIT)/examples/%.cpp
	$(CXX) $(MAIN_CPP) $(CXXFLAGS) -c -o $@ $<

$(IMGUI_GIT)/%.o:$(IMGUI_GIT)/%.cpp
	$(CXX) $(MAIN_CPP) $(CXXFLAGS) -c -o $@ $<

$(IMGUI_GIT)/examples/libs/gl3w/GL/%.o:$(IMGUI_GIT)/examples/libs/gl3w/GL/%.c
	$(CC) $(MAIN_CPP) $(CFLAGS) -c -o $@ $<

$(EXAMPLE_WORKER)/%.o:$(EXAMPLE_WORKER)/%.cpp
	$(CXX) $(EXAMPLE_WORKER_CPP) $(CXXFLAGS) -c -o $@ $<

$(THRIFT_WORKER)/%.o:$(THRIFT_WORKER)/%.cpp
	$(CXX) $(THRIFT_WORKER_CPP) $(CXXFLAGS) -c -o $@ $<

$(THRIFT_GENERATED)/%.o:$(THRIFT_GENERATED)/%.cpp
	$(CXX) $(THRIFT_WORKER_CPP) $(CXXFLAGS) -c -o $@ $<

$(THRIFT)/%.o:$(THRIFT)/%.cpp
	$(CXX) $(THRIFT_WORKER_CPP) $(CXXFLAGS) -c -o $@ $<
	
$(THRIFT)/*/%.o:$(THRIFT)/*/%.cpp
	$(CXX) $(THRIFT_WORKER_CPP) $(CXXFLAGS) -c -o $@ $<

all: $(EXAMPLE_WORKER_EXE) $(MAIN_EXE) $(THRIFT_WORKER_EXE)
	@echo Build complete for $(MAIN_EXE)

$(MAIN_EXE): $(MAIN_OBJS)
	$(CXX) -o $(OUTPUT)/$@ $^ $(MAIN_LDFLAGS)

$(EXAMPLE_WORKER_EXE): $(EXAMPLE_WORKER_OBJS)
	$(CXX) -o $(OUTPUT)/$@ $^ $(EXAMPLE_WORKER_LDFLAGS)

$(THRIFT_WORKER_EXE): $(THRIFT_WORKER_OBJS)
	$(CXX) -o $(OUTPUT)/$@ $^ $(THRIFT_WORKER_LDFLAGS)

clean:
	rm -f $(MAIN_EXE) $(MAIN_OBJS) $(EXAMPLE_WORKER_OBJS) $(THRIFT_WORKER_OBJS) *.js *.wasm *.wasm.pre $(OUTPUT)/*

execute:
	cd $(OUTPUT) ; emrun --browser=$(BROWSER) $(MAIN_EXE)
	
generate-thrift:
	thrift --gen cpp -out $(THRIFT_GENERATED) $(THRIFT_FILE)
