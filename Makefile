#paths
OUTPUT = ./out/
MAIN = ./src/
IMGUI_WEBCLIENT = ./src/imgui-webclient/
THRIFT_WORKER = ./src/thrift-worker/
QT_WEBCLIENT = ./src/qt-webclient/
THRIFT_GENERATED = $(THRIFT_WORKER)/gen-cpp/
THRIFT_GENERATED_QT = $(QT_WEBCLIENT)/gen-cpp/
IMGUI_GIT = ./libs/imgui/
THRIFT = ./libs/thrift/
EMS_SHELL = ./res/shell_minimal.html
THRIFT_FILE = ./res/CertificateGenerator.thrift

#compiler
CC = emcc --emrun
CXX = em++ --emrun
THRIFTC = thrift
DOCKER = docker
BROWSER = chrome

#dockerstuff
DOCKER_CONTAINER_BOOST = madmanfred/qt-webassembly-boost
DOCKER_CONTAINER_QT = madmanfred/qt-webassembly:qt5.14-em1.38.30
DOCKER_CONTAINER_IMGUI = trzeci/emscripten:sdk-tag-1.38.30-64bit
DOCKER_CONTAINER_THRIFT = cspwizard/thrift:0.13.0
DOCKER_RUN = $(DOCKER) run --rm -v $$(pwd):/src/ -u $$(id -u):$$(id -g) -w /src/

EXAMPLE_WORKER_EXE = worker.js
EXAMPLE_WORKER_SOURCES = $(EXAMPLE_WORKER)/worker.cpp
EXAMPLE_WORKER_OBJS = $(addsuffix .o, $(basename $(EXAMPLE_WORKER_SOURCES)))
EXAMPLE_WORKER_EMS = -s WASM=1
EXAMPLE_WORKER_EMS += -s ALLOW_MEMORY_GROWTH=0 -s BINARYEN_TRAP_MODE=clamp
EXAMPLE_WORKER_EMS += -s DISABLE_EXCEPTION_CATCHING=1 -s NO_EXIT_RUNTIME=0
EXAMPLE_WORKER_EMS += -s ASSERTIONS=1 -s NO_FILESYSTEM=1
EXAMPLE_WORKER_EMS += -s ASYNCIFY=1 -s BUILD_AS_WORKER=1 -s EXPORTED_FUNCTIONS='["_callback", "_main"]'
EXAMPLE_WORKER_CPP = -Wall -Wformat -O3 -std=c++17 $(EXAMPLE_WORKER_EMS)
EXAMPLE_WORKER_LDFLAGS = $(EXAMPLE_WORKER_EMS)

THRIFT_WORKER_EXE = thrift-worker.js
THRIFT_WORKER_SOURCES = $(THRIFT_WORKER)/worker.cpp
THRIFT_WORKER_SOURCES += $(THRIFT_GENERATED)/CertificateGenerator.cpp
THRIFT_WORKER_SOURCES += $(THRIFT_GENERATED)/CertificateGenerator_types.cpp
THRIFT_WORKER_SOURCES += $(THRIFT_GENERATED)/CertificateGenerator_constants.cpp
THRIFT_WORKER_SOURCES += $(THRIFT)/TApplicationException.cpp
THRIFT_WORKER_SOURCES += $(THRIFT)/TOutput.cpp
THRIFT_WORKER_SOURCES += $(THRIFT)/protocol/TProtocol.cpp
THRIFT_WORKER_SOURCES += $(THRIFT)/transport/TTransportException.cpp
THRIFT_WORKER_SOURCES += $(THRIFT)/transport/TSocket.cpp
THRIFT_WORKER_SOURCES += $(THRIFT)/transport/TBufferTransports.cpp
THRIFT_WORKER_OBJS = $(addsuffix .o, $(basename $(THRIFT_WORKER_SOURCES)))
THRIFT_WORKER_EMS = -s WASM=1
THRIFT_WORKER_EMS += -s ALLOW_MEMORY_GROWTH=1  -s BINARYEN_TRAP_MODE=clamp
THRIFT_WORKER_EMS += -s DISABLE_EXCEPTION_CATCHING=1 -s NO_EXIT_RUNTIME=1
THRIFT_WORKER_EMS += -s NO_FILESYSTEM=0
THRIFT_WORKER_EMS += -s ASYNCIFY=1 -s BUILD_AS_WORKER=1
THRIFT_WORKER_EMS += -s EXTRA_EXPORTED_RUNTIME_METHODS=FS
THRIFT_WORKER_EMS += -s EXPORTED_FUNCTIONS='["_tw_open", "_tw_close", "_tw_generateCertificates", "_main"]'
THRIFT_WORKER_CPP = -Wall -Wformat -Os -std=c++17
THRIFT_WORKER_CPP += -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H
THRIFT_WORKER_CPP += -I$(THRIFT_WORKER) -I$(THRIFT_GENERATED) -I$(THRIFT)/../
THRIFT_WORKER_CPP += $(THRIFT_WORKER_EMS)
THRIFT_WORKER_LDFLAGS = $(THRIFT_WORKER_EMS)

IMGUI_WEBCLIENT_EXE = index.html
IMGUI_WEBCLIENT_SOURCES = $(IMGUI_WEBCLIENT)/main.cpp
IMGUI_WEBCLIENT_SOURCES += $(IMGUI_GIT)/examples/imgui_impl_sdl.cpp $(IMGUI_GIT)/examples/imgui_impl_opengl3.cpp
IMGUI_WEBCLIENT_SOURCES += $(IMGUI_GIT)/imgui.cpp $(IMGUI_GIT)/imgui_demo.cpp $(IMGUI_GIT)/imgui_draw.cpp $(IMGUI_GIT)/imgui_widgets.cpp
IMGUI_WEBCLIENT_OBJS = $(addsuffix .o, $(basename $(IMGUI_WEBCLIENT_SOURCES)))
IMGUI_WEBCLIENT_EMS = --bind -s USE_SDL=2 -s WASM=1
IMGUI_WEBCLIENT_EMS += -s ALLOW_MEMORY_GROWTH=0 -s BINARYEN_TRAP_MODE=clamp
IMGUI_WEBCLIENT_EMS += -s DISABLE_EXCEPTION_CATCHING=1 -s NO_EXIT_RUNTIME=0
IMGUI_WEBCLIENT_EMS += -s ASSERTIONS=1 -s NO_FILESYSTEM=0
IMGUI_WEBCLIENT_EMS += -s ASYNCIFY=1 -s EXPORTED_FUNCTIONS='["_onerror", "_main"]'
IMGUI_WEBCLIENT_EMS += -s EXTRA_EXPORTED_RUNTIME_METHODS=FS
IMGUI_WEBCLIENT_CPP = -I$(IMGUI_GIT)/examples/ -I$(IMGUI_GIT)
IMGUI_WEBCLIENT_CPP += -Wall -Wformat -Os -std=c++17
IMGUI_WEBCLIENT_CPP += -DTHRIFT_WORKER_FILE=\"$(THRIFT_WORKER_EXE)\"
IMGUI_WEBCLIENT_CPP += $(IMGUI_WEBCLIENT_EMS)
IMGUI_WEBCLIENT_LDFLAGS = $(IMGUI_WEBCLIENT_EMS) --shell-file $(EMS_SHELL)

##---------------------------------------------------------------------
## BUILD RULES
##
## The internal-*** targets build locally without building the other targets
## The *** targets build locally
## The docker-*** targets build in containers, so you don't have to install the compilers.
##---------------------------------------------------------------------

all: docker-qt

$(IMGUI_WEBCLIENT)/%.o:$(IMGUI_WEBCLIENT)/%.cpp
	echo OJS: $(IMGUI_WEBCLIENT_OBJS) ENDS
	$(CXX) $(IMGUI_WEBCLIENT_CPP) $(CXXFLAGS) -c -o $@ $<

$(IMGUI_GIT)/examples/%.o:$(IMGUI_GIT)/examples/%.cpp
	$(CXX) $(IMGUI_WEBCLIENT_CPP) $(CXXFLAGS) -c -o $@ $<

$(IMGUI_GIT)/%.o:$(IMGUI_GIT)/%.cpp
	$(CXX) $(IMGUI_WEBCLIENT_CPP) $(CXXFLAGS) -c -o $@ $<

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

$(IMGUI_WEBCLIENT_EXE): $(OUTPUT)/$(IMGUI_WEBCLIENT_EXE)
$(EXAMPLE_WORKER_EXE): $(OUTPUT)/$(EXAMPLE_WORKER_EXE)
$(THRIFT_WORKER_EXE): $(OUTPUT)/$(THRIFT_WORKER_EXE)

$(OUTPUT)/$(IMGUI_WEBCLIENT_EXE): $(IMGUI_WEBCLIENT_OBJS)
	mkdir -p $(OUTPUT)
	$(CXX) -o $@ $^ $(IMGUI_WEBCLIENT_LDFLAGS)

$(OUTPUT)/$(EXAMPLE_WORKER_EXE): $(EXAMPLE_WORKER_OBJS)
	mkdir -p $(OUTPUT)
	$(CXX) -o $@ $^ $(EXAMPLE_WORKER_LDFLAGS)

$(OUTPUT)/$(THRIFT_WORKER_EXE): $(THRIFT_WORKER_OBJS)
	mkdir -p $(OUTPUT)
	$(CXX) -o $@ $^ $(THRIFT_WORKER_LDFLAGS)

#The plain targets link to the docker targets
thrift-worker: docker-thrift-worker

qt: docker-qt

imgui: docker-imgui

thrift: docker-thrift

clean: docker-clean

distclean: docker-distclean

#The execute targets start the applications
execute-qt:
	cd $(OUTPUT) ; emrun --browser=$(BROWSER) qt-webclient.html

execute-imgui:
	cd $(OUTPUT) ; emrun --browser=$(BROWSER) index.html

#The internal targets do the stuff
internal-thrift-worker:
	make $(THRIFT_WORKER_EXE)

internal-qt:
	cd $(QT_WEBCLIENT) ; qmake
	cd $(QT_WEBCLIENT) ; make
	mkdir -p $(OUTPUT)
	cp $(QT_WEBCLIENT)/{qt-webclient.html,qt-webclient.wasm,qt-webclient.js,qtloader.js,qtlogo.svg} $(OUTPUT)

internal-imgui:
	make $(IMGUI_WEBCLIENT_EXE)

internal-thrift:
	mkdir -p $(THRIFT_GENERATED)
	mkdir -p $(THRIFT_GENERATED_QT)
	$(THRIFTC) --gen cpp -out $(THRIFT_GENERATED) $(THRIFT_FILE)
	$(THRIFTC) --gen cpp -out $(THRIFT_GENERATED_QT) $(THRIFT_FILE)

internal-clean:
	rm -f $(MAIN_OBJS) $(EXAMPLE_WORKER_OBJS) $(THRIFT_WORKER_OBJS)
	- cd $(QT_WEBCLIENT) ; make clean

internal-distclean: clean
	rm -rf docker.gen thrift.gen $(OUTPUT)/* $(THRIFT_GENERATED) $(THRIFT_GENERATED_QT)
	- cd $(QT_WEBCLIENT) ; make distclean

#The local targets make with the locally installed dependencies
local-thrift-worker: local-thrift internal-thrift-worker

local-qt: local-thrift-worker local-thrift internal-qt

local-imgui: local-thrift-worker internal-imgui

local-thrift: internal-thrift

local-clean: internal-clean

local-distclean: local-clean internal-distclean

#The docker targets make in docker containers containing the dependencies
docker-thrift-worker: docker-thrift
	$(DOCKER_RUN) $(DOCKER_CONTAINER_BOOST) make internal-thrift-worker

docker-qt: docker-thrift-worker docker-thrift
	$(DOCKER_RUN) $(DOCKER_CONTAINER_QT) make internal-qt

docker-imgui: docker-thrift-worker
	$(DOCKER_RUN) $(DOCKER_CONTAINER_IMGUI) make internal-imgui

docker-thrift:
	make internal-thrift THRIFTC="$(DOCKER_RUN) $(DOCKER_CONTAINER_THRIFT)"

docker-clean:
	$(DOCKER_RUN) $(DOCKER_CONTAINER_QT) make internal-clean

docker-distclean: docker-clean
	$(DOCKER_RUN) $(DOCKER_CONTAINER_QT) make internal-distclean
