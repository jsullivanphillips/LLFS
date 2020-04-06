#include <stdio.h>
#include "../io/File.h"

int main(){
	initLLFS();
	makeDir("jhn");
	openFile("bob");
	write_file("okie doke so this is the deal i am suiper cool and wear the best sunglasses");
	write_file("i have forty-nine toes");
	clean();	
	return(0);
}
