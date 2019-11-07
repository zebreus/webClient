#include "supportfunctions.h"

#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <emscripten.h>

/** @brief Offers a file from IDBFS for download
* @param [in] filename a char* containing the path of the file on IDBFS
* @param [in] mimetype a char* containing the mimetype of the file
*
* Loads the file with filename from IDBFS and offers it for download
*/
EM_JS(void, downloadFile, (const char* filename, const char* mimetype), {
    console.log("A");
    let name = UTF8ToString(filename);
    let type = UTF8ToString(mimetype);
    console.log("B");
    FS.syncfs(true, function (err) {
        console.log("C");
        console.log(name);
        let content = FS.readFile(name,  { encoding: 'binary' });
        console.log("D");
        let a = document.createElement('a');
        var pos = name.lastIndexOf("/", name.length);
        a.download = name.slice(pos+1,name.length);
        console.log("E");
        a.href = URL.createObjectURL(new Blob([content], {type: type}));
        console.log("F");
        a.click();
        console.log("G");
        setTimeout(() => {
            URL.revokeObjectURL(a.href);
        }, 60000);
        console.log("H");
    });
});

void openPdf(std::string path)
{
    char* cFilename = new char[path.size()+1];
    std::memcpy(cFilename, path.c_str(), path.size()+1);
    downloadFile(cFilename,"application/pdf");
    delete[] cFilename;
}

void initFs()
{
    EM_ASM(
        // Mount IndexDBFS to /generated. This is, where the worker
        // will place the generated files
        console.log("INIT_A");
        FS.mkdir('/generated');
        FS.mount(IDBFS, {}, '/generated');
        FS.syncfs(true, function (err) {
             conosle.log("INIT_ERR");
        });
        console.log("INIT_B");
    );
}
