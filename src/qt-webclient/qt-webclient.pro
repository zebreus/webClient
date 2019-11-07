QT += quick

CONFIG += c++17

#Config for linux
!wasm-emscripten {
        QMAKE_CXXFLAGS += -std=c++0x -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H -Wall
        INCLUDEPATH += -I/usr/local/include/thrift
        LIBS+= -L/usr/local/lib -lthrift
}

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        certificategeneratobackend.cpp \
        main.cpp

#Sources for emscripten
wasm-emscripten {
        SOURCES += \
                emscripten/certificategeneratorconnection.cpp \
                emscripten/supportfunctions.cpp
}

#Sources for linux
!wasm-emscripten {
        SOURCES += \
                linux/certificategeneratorconnection.cpp \
                linux/supportfunctions.cpp \
                gen-cpp/CertificateGenerator.cpp \
                gen-cpp/CertificateGenerator_constants.cpp \
                gen-cpp/CertificateGenerator_types.cpp
}

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    certificategeneratobackend.h \
    certificategeneratorconnection.h \
    exampledata.h \
    supportfunctions.h

#Headers for emscripten
#wasm-emscripten {
#        HEADERS += \
#                emscripten/certificategeneratorconnection.h \
#                emscripten/supportfunctions.h
#}

#Headers for linux
!wasm-emscripten {
        HEADERS += \
#                linux/certificategeneratorconnection.h \
#                linux/supportfunctions.h \
                gen-cpp/CertificateGenerator.h \
                gen-cpp/CertificateGenerator_constants.h \
                gen-cpp/CertificateGenerator_types.h \
}
