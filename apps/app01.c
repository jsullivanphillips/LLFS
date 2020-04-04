#include <stdio.h>
#include "../io/File.h"

int main(){
	initLLFS();
	formatDisk();
	printFreeBlock();
	findOpenBlock();
	markBlockTaken(20);
	printFreeBlock();


	return(0);
}
