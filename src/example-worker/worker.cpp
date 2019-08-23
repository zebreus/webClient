#include <stdio.h>
#include <emscripten.h>
#include <string>

using namespace std;
//using namespace emscripten;

void main_cpp(int, char**);

extern "C"
{
int finished = 0;

void callback(char* data, int size){
	char working[8] = "WORKING";
	char finishedText[9] = "FINISHED";
	if(finished == 0){
		emscripten_worker_respond(working , 8);
	}else{
		emscripten_worker_respond(finishedText ,9);
	}
}


int main(int i, char** c){
	main_cpp(i, c);
}
}

void main_cpp(int, char**)
{
	EM_ASM( console.log("Worker started"); );
	for(int x = 0; x < 10; x++){
		emscripten_sleep(500);
		EM_ASM( console.log("Working"); );
    }
    EM_ASM( console.log("Work finished"); );
	finished = 1;
}

void main_loop(void* arg)
{
	
}

