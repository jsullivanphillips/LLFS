#include <stdio.h>
#include "../io/File.h"

int main(){
	initLLFS();
	formatDisk();
	createFile("jim");
	createFile("sam");	
	return(0);
}
