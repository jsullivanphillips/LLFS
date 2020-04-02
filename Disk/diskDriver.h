#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>


const int BLOCK_SIZE = 512;
const int NUM_BLOCKS = 4096;
const char DISKNAME[12] = "vdisk";
void readBlock(FILE* disk, int blockNum, char* buffer){
	fseek(disk, blockNum * BLOCK_SIZE, SEEK_SET);
	fread(buffer, BLOCK_SIZE, 1, disk);
}

void writeBlock(FILE* disk, int blockNum, int offset, char* data){
	fseek(disk, (blockNum * BLOCK_SIZE)+offset, SEEK_SET);
	fwrite(data, BLOCK_SIZE, 1, disk); //will overwrite exisitng data
}

void createDisk(){
	int i;
	char *buffer = malloc(BLOCK_SIZE);
	FILE* disk = fopen(DISKNAME, "w");
	memset(buffer, 0, BLOCK_SIZE);
	for(i = 0; i < NUM_BLOCKS; i++){
		fwrite(buffer, BLOCK_SIZE, 1, disk);
		fseek(disk, i * BLOCK_SIZE, SEEK_SET);
	}
	
	printf("value of i: %d\n", i);
	fclose(disk);
	free(buffer);	


	printf("created virtual disk %s of size= %d\n", DISKNAME, BLOCK_SIZE*NUM_BLOCKS);
}
