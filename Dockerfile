FROM trzeci/emscripten:sdk-tag-1.38.30-64bit
MAINTAINER Lennart E.

RUN mkdir -p /tmp/boost ;\
	cd /tmp/boost ;\
	wget -Oboost.tar.bz2 https://vorboss.dl.sourceforge.net/project/boost/boost/1.66.0/boost_1_66_0.tar.bz2 ;\
	tar xjf boost.tar.bz2 ;\
	cd /tmp/boost/boost*/ ;\
	./bootstrap.sh ;\
	./b2 toolset=emscripten cxxflags=-std=c++17 system ;\
	mv /tmp/boost/boost_*/boost /emsdk_portable/sdk/system/include/ ;\
	rm -rf /tmp/boost



