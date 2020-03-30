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
	FILE* disk = fopen(VDISK_PATH, "r+");
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

void readB(int blockNum, char *buff){
	FILE *disk = fopen(VDISK_PATH, "r");
	readBlock(disk, blockNum, buff);
	fclose(disk);
}

void writeB(int blockNum, char *buff){
	FILE *disk = fopen(VDISK_PATH, "r+");
	writeBlock(disk, blockNum, buff);
	fclose(disk);
}

//creates a file with given file name
//Returns 0 for succesfull, 1 for error
int createFile(char *filename){
	int blockNum = findOpenBlock();
	printf("found open block %d \n", blockNum);		


	return(0);
}

//returns an int to an available block
//returns value of open block (>10)
//if no block found returns 1
int findOpenBlock(){
	char *buffer;
	int i = 0;
	buffer = (char *)malloc(BLOCK_SIZE);
	readB(1, buffer);
	while(buffer[i] != 1){
		printf("index: %d, value: %d \n", i, buffer[i]);
		if( i == BLOCK_SIZE){
			printf("no open block found\n");
			return(1);
		}
		i++;
	}
	free(buffer);
	markBlockTaken(i);
	return (i);
}

void markBlockTaken(int blockNum){
	char *buffer;
	buffer = (char *)malloc(BLOCK_SIZE);
	readB(1, buffer);
	buffer[blockNum] = 0;
	writeB(1, buffer);
	printf("free vector index: %d, marked taken\n", blockNum);
	free(buffer);	

}











