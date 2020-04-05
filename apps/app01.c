#include <stdio.h>
#include "../io/File.h"

int main(){
	initLLFS();
	makeDir("jhn");
	makeDir("poopy");
	makeDir("nun");
	openFile("bob");
	openFile("bob");
	clean();	
	return(0);
}
