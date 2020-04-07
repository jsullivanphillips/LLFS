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
#define MAX_LOG_BLOCKS 1
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
int cur_dir_inode;
char entry_name[16][31];
char entry_inode[16];
char cur_dir_name[31];
char SELF_DIR[31] = ".\0";
char PREV_DIR[31] = "..\0";
//LOG
char *LOG;
int logQuantity;
int logLocations[MAX_LOG_BLOCKS];
//FILE
int cur_file_inode;
char cur_file_name[31];
int cur_file_size;
int cur_file_blocks[10];

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
	memset(buffer, 0, BLOCK_SIZE);
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


//change directory 
//delete directory 
//delete file
void openFile(char *fileName){
	if(existsInDir(fileName) != -1){
		cur_file_inode = existsInDir(fileName);
		strncpy(cur_file_name, fileName, 31);
		get_cur_file_size();
		get_cur_file_blocks();
	}else{
		int m = findOpenBlock();
		int inode_val = create_inode(m);
		addToCurrDir(fileName, inode_val);
		cur_file_inode = inode_val;
		cur_file_size = 0;
		get_cur_file_blocks();
		strncpy(cur_file_name, fileName, 31);
		char *buffer = malloc(BLOCK_SIZE);
		memset(buffer, 0, BLOCK_SIZE);
		writeLog(m, buffer);
		free(buffer);
	}
}

void get_cur_file_size(){
	FILE *disk = fopen(VDISK_PATH, "rb+");
	char *buffer = malloc(BLOCK_SIZE);
	readBlock(disk, cur_file_inode, buffer);
	memcpy(&cur_file_size, buffer, 4);

	fclose(disk);
	free(buffer);
}

void get_cur_file_blocks(){
	pushLog();
	FILE *disk = fopen(VDISK_PATH, "rb+");
	char *buffer = malloc(BLOCK_SIZE);
	memset(buffer, 0, BLOCK_SIZE);
	readBlock(disk, cur_file_inode, buffer);
	for(int i = 0; i <10; i++){
		memcpy(&cur_file_blocks[i], buffer + 8 + (i * 2), 2);
	}

	fclose(disk);
	free(buffer);
}



void cd(char* dirName){

	int inode_block = existsInDir(dirName);
	if(inode_block == -1){
		printf("%s not in current directory \n", dirName);
		return;
	}	
	FILE *disk = fopen(VDISK_PATH, "rb+");
	char *buffer = malloc(BLOCK_SIZE);
	memset(buffer, 0, BLOCK_SIZE);
	readBlock(disk, inode_block, buffer);
	memcpy(&cur_dir_block, buffer +8, 2);
	free(buffer);

	char *dir_buffer = malloc(BLOCK_SIZE);
	memset(dir_buffer, 0, BLOCK_SIZE);
	readBlock(disk, cur_dir_block, dir_buffer);
	for(int i =0; i < 16; i++){
		memcpy(&entry_inode[i], dir_buffer + (i * 32), 1);
		strncpy(entry_name[i], dir_buffer + (i * 32)+1, 31);
	}
	strncpy(cur_dir_name, dirName, 31);	
	printf("/%s  \n", cur_dir_name);		


}


void write_file(char *contents){
	int num_old_blocks, num_new_blocks, total_blocks, j;
	if(cur_file_size == 0){
		num_old_blocks = 0;
	}else{
		num_old_blocks = (cur_file_size/BLOCK_SIZE) +1;
	}
	num_new_blocks = (strlen(contents) / BLOCK_SIZE)+1;
	



	char *buffer = malloc(BLOCK_SIZE);	
	memset(buffer, 0, BLOCK_SIZE);
	total_blocks = num_new_blocks + num_old_blocks;
	j = 0;
	for(int i = num_old_blocks; i < total_blocks; i++){
		cur_file_blocks[i] = findOpenBlock();
		memcpy(buffer, contents + ( j * BLOCK_SIZE ), BLOCK_SIZE);
		memset(buffer + strlen(contents), 0, BLOCK_SIZE - strlen(contents));
		writeLog(cur_file_blocks[i], buffer);
		j++;
	}
	for(int i = total_blocks; i < 10; i++){
		cur_file_blocks[i] = 0;
	}	
	
	free(buffer);
	cur_file_size = strlen(contents) + cur_file_size;
	cur_file_inode = update_inode(cur_file_inode, cur_file_blocks, cur_file_size);
	updateDir(cur_file_name, cur_file_inode);

	

}






int update_inode(int cur_file_inode, int blocks[], int size_of_contents){
	char *buffer = malloc(BLOCK_SIZE);
	memset(buffer, 0, BLOCK_SIZE);
	memcpy(buffer, &size_of_contents, 4);
	memset(buffer + 4, 0, 4);
	for(int i = 0; i < 10; i++){
		memcpy(buffer + 8 + (i * 2), &blocks[i], 2);
	}
	obsolete_blocks[num_of_obsolete] = cur_file_inode;
	num_of_obsolete++;
	
	int updated_inode_block = findOpenBlock();
	writeLog(updated_inode_block, buffer);
	free(buffer);
	return updated_inode_block;

}

void updateDir(char *fileName, int blockNum){
	for(int i = 0; i < 16; i++){
		if(entry_inode[i] != 0){
			if(strcmp(fileName, entry_name[i]) == 0){
				entry_inode[i] = blockNum;
			}
		}
	}
}

int existsInDir(char *fileName){
	for(int i =0; i <16; i++){
		if(entry_inode[i] != 0){
			if(strcmp(fileName, entry_name[i]) == 0){
				return(entry_inode[i]);
			}
		}
	}
	return -1;
}


void addToCurrDir(char *dirName, int inode_val){
	int index = 16 - spaceInCurDir();
	strcpy(entry_name[index], dirName);
	entry_inode[index] = inode_val;
	char *buffer = malloc(BLOCK_SIZE);
	memset(buffer, 0, BLOCK_SIZE);
	for(int i = 0; i < 16; i ++){
		if(entry_inode[i] != 0){
			memcpy(buffer + (i * 32), &entry_inode[i], 1);
			memcpy(buffer + (i * 32) + 1, &entry_name[i], 31);
		}
	}
	//mark old block free in vector
	obsolete_blocks[num_of_obsolete] = cur_dir_block;
	num_of_obsolete++;
	cur_dir_block = findOpenBlock();
	writeLog(cur_dir_block, buffer);
	free(buffer);
}

void makeDir(char *dirName){
	if(spaceInCurDir()){
		int m = findOpenBlock();
		int inode_val = create_inode(m);
		addToCurrDir(dirName, inode_val);
		char *dir = malloc(BLOCK_SIZE);
		memset(dir, 0, 	BLOCK_SIZE);
		memcpy(dir, &inode_val, 1);
		memcpy(dir+1, &SELF_DIR, 31);
		memcpy(dir+32, &cur_dir_inode, 1);
		memcpy(dir+33, &PREV_DIR, 31);
		writeLog(m, dir);
		free(dir);
	}else{
		printf("no space in %s, please rm to make room for new directory\n", cur_dir_name);
	}
	

}
	


int create_inode(int blockNum){
	int m = findOpenBlock();

	char *inode = malloc(BLOCK_SIZE);
	memset(inode, 0, BLOCK_SIZE);
	memset(inode, 0, 32);
	memcpy(inode + 8, &blockNum, 2);
	writeLog(m, inode);
	free(inode);	
	return m;
}

void clean(){
	pushLog();
	free(LOG);
}

void initLLFS(){
	createDisk();
	LOG = calloc(MAX_LOG_BLOCKS, BLOCK_SIZE);
	logQuantity = 0;
	memset(entry_inode, 0, 16);
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
	printf("created disk of %dMb\n", (BLOCK_SIZE * NUM_BLOCKS)/1000000);
		
	cur_dir_inode = entry_inode[0];}

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
	strcpy(entry_name[0], SELF_DIR);
	entry_inode[0] = create_inode(cur_dir_block);
	cur_dir_inode = entry_inode[0];
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







