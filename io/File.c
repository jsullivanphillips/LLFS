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
#define BLOCK_SIZE_BYTES 512
#define MAX_FILENAME_SIZE 128
#define NUM_BLOCKS 4096
#define NUM_INODES 128 //changeable up, to design decision
#define MAGIC_NUM 42

//FILE SYSTEM










//***HIGHER LEVEL DISK-DRIVER INTERACTIONS***
void initLLFS(){
	createDisk();	
}


void formatDisk(){
	//SUPERBLOCK
	char* buffer;
	FILE* disk = fopen(VDISK_PATH, "r+b");
	buffer = malloc(BLOCK_SIZE);
	int magic = MAGIC_NUM;
	int blocks = NUM_BLOCKS;
	int inodes = NUM_INODES;
	memset(buffer, 0, BLOCK_SIZE);
	memcpy(buffer, &magic, 4);
	memcpy(buffer + 4, &blocks, 4);
	memcpy(buffer + 8, &inodes, 4);
	writeBlock(disk, 0, buffer);

	//FREE VECTOR BLOCK
	memset(buffer, 63, 1);
	memset(buffer+1, 255, 511);
	writeBlock(disk, 1, buffer);
	free(buffer);
	fclose(disk);

}

void readB(int blockNum, char *buff){
	FILE *disk = fopen(VDISK_PATH, "r");
	readBlock(disk, blockNum, buff);
	fclose(disk);
}

void writeB(int blockNum, int offset, char *buff){
	FILE *disk = fopen(VDISK_PATH, "r+");
	writeBlock(disk, blockNum, buff);
	fclose(disk);
}

//returns an int to an available block
//returns value of open block (>10)
//if no block found returns -1
int findOpenBlock(){
	int i = 10;
	unsigned char buffer[BLOCK_SIZE_BYTES];
	FILE *fp = fopen(VDISK_PATH, "rb");
	readBlockB(fp, 1, buffer);
	for(i= 0; i <= 512; i++){
		for(int j = 0; j < 8; j++){
			int k = buffer[i] >> j;
			if(k & 1){
				fclose(fp);
				return(i + j + 1);
			}
		}
		printf("\n");
	}

	fclose(fp);
	return (-1);
}



void markBlockTaken(int blockNum){
	printf("marking block %d as taken \n", blockNum);
	FILE *fp = fopen(VDISK_PATH, "rb+");
	unsigned char buffer[BLOCK_SIZE_BYTES];
	readBlockB(fp, 1, buffer);
	int index = blockNum / 8;
	int bit_index = blockNum % 8;
	buffer[index] &= ~(1UL << bit_index); 
	writeBlockB(fp, 1, buffer);
	fclose(fp);
}


//debuggin function
void printFreeBlock(){
	FILE *fp = fopen(VDISK_PATH, "rb");
	unsigned char buffer[BLOCK_SIZE_BYTES];
	readBlockB(fp, 1, buffer);
	printf("%04x: ", 0);
	for(int i = 1; i <= 512; i++){
		printf("%02x ", buffer[i-1]);
		if(i % 8 == 0){
			printf("\n");
			printf("%04x: ", i);
		}
	}
	printf("\n");
	fclose(fp);
}




