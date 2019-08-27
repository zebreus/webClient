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

RUN apt update ;\
	apt install -y libtool automake autoconf flex bison pkg-config ;\
	mkdir -p /tmp/thrift/ ;\
	cd /tmp/thrift/ ;\
	git clone https://github.com/apache/thrift.git ;\
	cd /tmp/thrift/thrift/ ;\
	./bootstrap.sh ;\
	./configure --enable-libs=no --enable-tests=no --enable-tutorial=no --bindir=/usr/bin;\
	make ;\
	make install ;\
	rm -rf /tmp/thrift ;\
	apt remove -y libtool automake pkg-config

