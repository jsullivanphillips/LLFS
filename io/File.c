#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../Disk/diskDriver.h"
#include "File.h"

#define VDISK_PATH "../apps/vdisk"
#define BLOCK_SIZE 4096
#define MAX_FILENAME_SIZE 128
#define NUM_BLOCKS 4096
#define NUM_INODES 128 //changeable up, to design decision

void initLLFS(){
	createDisk();	
}

void formatDisk(){
	//SUPERBLOCK
	char* buffer;
	FILE* disk = fopen(VDISK_PATH, "rb");
	buffer = (char *)malloc(BLOCK_SIZE);
	int magic = 42;
	int blocks = NUM_BLOCKS;
	int inodes = NUM_INODES;
	memcpy(buffer, &magic, sizeof(magic));
	memcpy(buffer + sizeof(int) *1, &blocks, sizeof(int));
	memcpy(buffer + sizeof(int) *2, &inodes, sizeof(int));
	writeBlock(disk, 0, buffer);

	//FREE VECTOR BLOCK
	memset(buffer, 1, BLOCK_SIZE);
	memset(buffer, 0, 10);
	printf("%s\n", buffer);
	writeBlock(disk, 1, buffer);
	free(buffer);
	fclose(disk);

}

void readB(int blockNum){
	char* buffer;
	FILE* disk = fopen(VDISK_PATH, "r");
	buffer = (char *)malloc(BLOCK_SIZE);
	readBlock(disk, blockNum, buffer);
	printf("%s\n", buffer);
	free(buffer);
}
