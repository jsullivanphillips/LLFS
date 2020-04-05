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

void writeBlock(FILE* disk, int blockNum, char* data){
	fseek(disk, (blockNum * BLOCK_SIZE), SEEK_SET);
	fwrite(data, BLOCK_SIZE, 1, disk); //will overwrite exisitng data
}

void readBlockB(FILE* disk, int blockNum, unsigned char* buffer){
	fseek(disk, blockNum * BLOCK_SIZE, SEEK_SET);
	fread(buffer, BLOCK_SIZE, 1, disk);
}


void writeBlockB(FILE* disk, int blockNum, unsigned char* data){
	fseek(disk, (blockNum * BLOCK_SIZE), SEEK_SET);
	fwrite(data, BLOCK_SIZE, 1, disk); //will overwrite exisitng data
}


