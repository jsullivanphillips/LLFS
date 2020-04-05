#include <stdio.h>
#include "../io/File.h"

int main(){
	initLLFS();
	findOpenBlock();
	findOpenBlock();
	findOpenBlock();
	findOpenBlock();
	updateFreeBlock();	
	return(0);
}
