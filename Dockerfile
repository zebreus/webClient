FROM trzeci/emscripten:sdk-tag-1.38.30-64bit
MAINTAINER Lennart E.

#Install boost headers in emscripten directory
RUN mkdir -p /tmp/boost ;\
	cd /tmp/boost ;\
	wget -Oboost.tar.bz2 https://vorboss.dl.sourceforge.net/project/boost/boost/1.66.0/boost_1_66_0.tar.bz2 ;\
	tar xjf boost.tar.bz2 ;\
	cd /tmp/boost/boost*/ ;\
	./bootstrap.sh ;\
	./b2 toolset=emscripten cxxflags=-std=c++17 system ;\
	mv /tmp/boost/boost_*/boost /emsdk_portable/sdk/system/include/ ;\
	rm -rf /tmp/boost

#Install the newest version of thrift
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

#Tnstall qt for webassembly
RUN apt update && apt install git ;\
	git clone git://code.qt.io/qt/qt5.git ;\
	cd qt5 ; git checkout 5.13 ;\
	cd qt5 ; ./init-repository --module-subset=qtbase,qtdeclarative,qtquickcontrols2,qtwebsockets,qtsvg,qtcharts,qtgraphicaleffects,qtxmlpatterns -f ;\
	cd qt5 ; ./configure -xplatform wasm-emscripten -nomake examples -prefix /qtbase -c++std c++17 -opensource -confirm-license ;\
	cd qt5 ; make module-qtbase module-qtsvg module-qtdeclarative module-qtwebsockets module-qtgraphicaleffects module-qtxmlpatterns module-qtquickcontrols2 module-qtcharts ;\
	cd qt5 ; make install ;\
	rm -rf qt5

#Add qt to path
ENV PATH="/qtbase/bin:${PATH}"
