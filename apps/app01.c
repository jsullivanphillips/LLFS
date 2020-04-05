#include <stdio.h>
#include "../io/File.h"

int main(){
	initLLFS();
	formatDisk();
	findOpenBlock();
	clean();
	return(0);
}
