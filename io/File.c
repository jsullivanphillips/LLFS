#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../Disk/diskDriver.h"
#include "File.h"

#define VDISK_PATH "../apps/vdisk"
#define BLOCK_SIZE 512
#define NUM_BLOCKS 4096
#define NUM_INODES 128
#define MAGIC_NUM 42

unsigned char bitMap[NUM_BLOCKS];
unsigned char freeBlock[BLOCK_SIZE];



void initLLFS(){
	createDisk();
	formatDisk();
}

void createDisk(){
	int i;
	char *buffer = malloc(BLOCK_SIZE);
	FILE *disk = fopen(VDISK_PATH, "w");
	memset(buffer, 0, BLOCK_SIZE);
	for(i = 0; i < NUM_BLOCKS; i++){
		fseek(disk, i * BLOCK_SIZE, SEEK_SET);
		fwrite(buffer, BLOCK_SIZE, 1, disk);
	}

	fclose(disk);
	free(buffer);
	printf("created disk of %dkb\n", (BLOCK_SIZE * NUM_BLOCKS)/1000);
		
}

void formatDisk(){
	unsigned char* buffer;
	FILE *disk = fopen(VDISK_PATH, "rb+");
	buffer = malloc(BLOCK_SIZE);
	int magic = MAGIC_NUM;
	int blocks = NUM_BLOCKS;
	int inodes = NUM_INODES;
	memset(buffer, 0, BLOCK_SIZE);
	memcpy(buffer, &magic, 4);
	memcpy(buffer + 4, &blocks, 4);
	memcpy(buffer + 8, &inodes, 4);
	writeBlockB(disk, 0, buffer);

	//FREE VECTOR BLOCK
	memset(freeBlock, 255, BLOCK_SIZE);
	memset(freeBlock, 0, 1);
	memset(freeBlock + 1, 63, 1);
	writeBlockB(disk, 1, freeBlock);
	memset(bitMap, 0, NUM_BLOCKS);
	memset(bitMap + 10, 1, NUM_BLOCKS - 10);




	free(buffer);
	fclose(disk);

}


int findOpenBlock(){
	for(int i = 0; i < NUM_BLOCKS; i++){
		if(bitMap[i] == 1){
			printf("block %d found open\n", i);
			bitMap[i] = 0;
			int index = i / 8;
			int bit_index = i % 8;
			freeBlock[index] &= ~(1UL <<(7- bit_index));
			return i;
		}
	}
	printf("ERROR; no open block found");
	return -1;
}

void updateFreeBlock(){
	FILE *disk = fopen(VDISK_PATH, "rb+");
	writeBlockB(disk, 1, freeBlock);
	fclose(disk);

}







