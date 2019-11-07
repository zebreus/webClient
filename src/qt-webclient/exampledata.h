#ifndef EXAMPLEDATA_H
#define EXAMPLEDATA_H

#include <string>
using namespace std;

static string exampleData = "{\
\"students\":[\
    {\
    \"name\":\"Max\",\
    \"surname\":\"Mustermann\",\
    \"date\":\"14. August 1998\",\
    \"tasks\":[\
        {\
        \"name\":\"showing up\",\
        \"grade\":\"2.3\"\
        },\
        {\
        \"name\":\"breathing\",\
        \"grade\":\"2.7\"\
        }\
    ]\
    },\
    {\
    \"name\":\"Tim\",\
    \"surname\":\"Testperson\",\
    \"date\":\"8. April 2019\",\
    \"tasks\":[\
        {\
        \"name\":\"showing up\",\
        \"grade\":\"1.0\"\
        },\
        {\
        \"name\":\"not dying\",\
        \"grade\":\"1.0\"\
        },\
        {\
        \"name\":\"breathing\",\
        \"grade\":\"1.0\"\
        }\
    ]\
    }\
],\
\"tester\":\"Someone Important\"\
}\
";
//example_template_1.tex aus dem certificate-generator
static string exampleTemplate =
"\\documentclass[a4paper]{article}\n\
\\usepackage[utf8]{inputenc}\n\
\\usepackage[T1]{fontenc}\n\
\\usepackage{lmodern}\n\
\\usepackage[german]{babel}\n\
\\usepackage{amsmath}\n\
\\usepackage{certificate-generator}\n\
\\usepackage{ifthen}\n\
\\title{Certificate of Existence}\n\
\\author{\\substitude{name}}\n\
\\date{\\substitude{date}}\n\
\\begin{document}\n\
\\maketitle\n\
\\section{Certificate}\n\
\\subsection{Text}\n\
\\substitude[student]{name} \\substitude{surname} you are a great person and on \\substitude{date} you have proven your existence by completing the following tasks:\n\
\\subsection{Tasks}\n\
\\begin{tabular}{ l | c }\n\
\\hline\n\
Task & Rating \\\\ \\hline\n\
\\optional{tasks}{\n\
    \\substitude{name} & \\substitude{grade} \\\\ \\hline\n\
}\n\
\\end{tabular}\n\
\\subsection{Tester}\n\
Certified by \\substitude{tester}\n\
\\end{document}";

#endif // EXAMPLEDATA_H
