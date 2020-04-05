#include <stdio.h>
#include "../io/File.h"

int main(){
	initLLFS();
	makeDir("jhn");
	makeDir("poopy");
	makeDir("nun");
	clean();	
	return(0);
}
