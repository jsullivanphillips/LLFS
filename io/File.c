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
#define MAX_LOG_BLOCKS 2
//GLOBAL VARS
//BITMAP
char bitMap[NUM_BLOCKS];
char freeBlock[BLOCK_SIZE];
int obsolete_blocks[NUM_BLOCKS];
int num_of_obsolete = 0;
//INODES
int imap[NUM_INODES];
char imap_block[NUM_INODES * 4];
char num_rem_inodes = NUM_INODES - 1;
//DIRECTORIES
char cur_dir_block;
char entry_name[16][31];
char entry_inode[16];
char cur_dir_name[31];
char SELF_DIR[31] = ".\0";
//LOG
char *LOG;
int logQuantity;
int logLocations[MAX_LOG_BLOCKS];




void writeLog(int blockNum,  char* buffer){
	memcpy(LOG + (logQuantity * BLOCK_SIZE), buffer, BLOCK_SIZE);
	logLocations[logQuantity] = blockNum;
	logQuantity++;
	if(logQuantity == MAX_LOG_BLOCKS){
		pushLog();
	}
}	




void pushLog(){
	updateFreeBlock();
	FILE *disk = fopen(VDISK_PATH, "rb+");
	char* buffer = malloc(BLOCK_SIZE);
	for(int i = 0; i < logQuantity; i++){
		memcpy(buffer, &LOG[i * BLOCK_SIZE], BLOCK_SIZE);
		writeBlock(disk, logLocations[i], buffer);
	}
	fclose(disk);
	free(buffer);
	logQuantity = 0;
}


int spaceInCurDir(){
	int j = 0;
	for(int i =0; i < 16; i++){
		if(entry_inode[i] == 0){
			j++;
		}
	}
	return j;
}

void addToCurrDir(char *dirName, int inode_val){
	int index = 16 - spaceInCurDir();
	printf("%d index for %s\n", index, dirName);
	strcpy(entry_name[index], dirName);
	entry_inode[index] = inode_val;
	char *buffer = malloc(BLOCK_SIZE);
	for(int i = 0; i < 16; i ++){
		if(entry_inode[i] != 0){
			printf("adding %s to %s \n", entry_name[i], cur_dir_name);
			memcpy(buffer + (i * 32), &entry_inode[i], 1);
			memcpy(buffer + (i * 32) + 1, &entry_name[i], 31);
		}
	}
	//mark old block free in vector
	obsolete_blocks[num_of_obsolete] = cur_dir_block;
	num_of_obsolete++;
	printf("finding new block for %s \n", cur_dir_name);
	cur_dir_block = findOpenBlock();
	writeLog(cur_dir_block, buffer);
}

void makeDir(char *dirName){
	if(spaceInCurDir()){
		printf("finding block for %s \n", dirName);
		int m = findOpenBlock();
		printf("finding block for inode for %s \n", dirName);
		int inode_val = create_inode(m);
		addToCurrDir(dirName, inode_val);
		char *dir = malloc(BLOCK_SIZE);
		memset(dir, 0, 	BLOCK_SIZE);
		memcpy(dir, &inode_val, 1);
		memcpy(dir+1, &SELF_DIR, 31);
		writeLog(m, dir);
	}else{
		printf("no space in %s, please rm to make room for new directory\n", cur_dir_name);
	}
	

}
	


int create_inode(int blockNum){
	int m = findOpenBlock();
	int inode_num = NUM_INODES - num_rem_inodes;
	imap[inode_num] = m;
	num_rem_inodes--;

	char *inode = malloc(BLOCK_SIZE);
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
	printf("finished initializing LLFS.. \n");
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
	char* buffer;
	FILE *disk = fopen(VDISK_PATH, "rb+");
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
	memset(freeBlock, 255, BLOCK_SIZE);
	memset(freeBlock, 0, 1);
	memset(freeBlock + 1, 63, 1);
	writeBlock(disk, 1, freeBlock);
	memset(bitMap, 0, NUM_BLOCKS);
	memset(bitMap + 10, 1, NUM_BLOCKS - 10);

	//Root Directory
	//resetting buffer
	memset(buffer, 0, BLOCK_SIZE);
	//setting up local cur_dir vars
	cur_dir_block = findOpenBlock();
	strcpy(cur_dir_name,"/\0");
	strcpy(entry_name[0], cur_dir_name);
	entry_inode[0] = create_inode(cur_dir_block);

	memcpy(buffer, &entry_inode[0], 1);
	memcpy(buffer+1, &SELF_DIR, 31);
	writeBlock(disk, cur_dir_block, buffer);


	

	free(buffer);
	fclose(disk);

}
void markBlockFree(int blockNum){
	bitMap[blockNum] = 1;
	int index = blockNum /8;
	int bit_index = blockNum % 8;
	freeBlock[index] |= (1UL << (8 - bit_index));

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
	writeBlock(disk, 1, freeBlock);
	fclose(disk);

}







