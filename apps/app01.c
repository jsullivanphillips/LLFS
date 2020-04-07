#include <stdio.h>
#include "../io/File.h"

int main(){
	initLLFS();
	makeDir("firstFolder");
	openFile("fileOne");
	makeDir("secondFolder");
	write_file("this is a very special file with very special contents!");
	readFile();
	cd("firstFolder");
	openFile("secretFile");
	write_file("this is a secret file with secret contents");
	return(0);
}
