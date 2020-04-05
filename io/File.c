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
#define MAX_LOG_BLOCKS 5

unsigned char bitMap[NUM_BLOCKS];
unsigned char freeBlock[BLOCK_SIZE];
int imap[NUM_INODES];
unsigned char imap_block[NUM_INODES * 4];
unsigned char num_rem_inodes = NUM_INODES - 1;
unsigned char cur_dir;
unsigned char *LOG;
int logQuantity;
int logLocations[MAX_LOG_BLOCKS];

void writeLog(int blockNum, unsigned char* buffer){
	memcpy(LOG + (logQuantity * BLOCK_SIZE), buffer, BLOCK_SIZE);
	logLocations[logQuantity] = blockNum;
	logQuantity++;
	if(logQuantity == MAX_LOG_BLOCKS){
		pushLog();
	}
}

void pushLog(){
	FILE *disk = fopen(VDISK_PATH, "rb+");
	unsigned char* buffer = malloc(BLOCK_SIZE);
	for(int i = 0; i < logQuantity; i++){
		memcpy(buffer, &LOG[i * BLOCK_SIZE], BLOCK_SIZE);
		writeBlockB(disk, logLocations[i], buffer);
	}
	fclose(disk);
	free(buffer);
	logQuantity = 0;
}
	


unsigned char create_inode(int blockNum){
	int m = findOpenBlock();
	unsigned char inode_num = NUM_INODES - num_rem_inodes;
	imap[inode_num] = m;
	num_rem_inodes--;

	unsigned char *inode = malloc(BLOCK_SIZE);
	memset(inode, 0, 32);
	memcpy(inode + 8, &blockNum, 2);
	writeLog(m, inode);
	free(inode);	
	return inode_num;
}

void clean(){
	free(LOG);
}

void initLLFS(){
	createDisk();
	LOG = malloc(BLOCK_SIZE * MAX_LOG_BLOCKS);
	logQuantity = 0;
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

	//Root Directory
	memset(buffer, 0, BLOCK_SIZE);
	cur_dir = 3;
	char cur_dir_name[31] = ".\0";
	unsigned char inode_val = create_inode(3);
	memcpy(buffer, &inode_val, 1);
	memcpy(buffer+1, &cur_dir_name, 31);
	writeBlockB(disk, 3, buffer);


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







