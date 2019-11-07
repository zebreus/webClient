#include "supportfunctions.h"

#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <stdlib.h>
#include <QProcess>
#include <QString>
#include <string>

using namespace std;

void openPdf(string path)
{
//    int childId = vfork();
//    if(childId == 0){
//        string command = "evince ";
//        command.append(path);

//        //system(command.c_str());
//    }
    string command = "evince ";
    command.append(path);
    QProcess::startDetached(QString::fromStdString(command));
}

void initFs()
{
    //Only needed in emscripten
}
