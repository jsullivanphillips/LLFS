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
	FILE* disk = fopen(VDISK_PATH, "r+b");
	buffer = (char *)malloc(BLOCK_SIZE);
	int magic = 16;
	int blocks = NUM_BLOCKS;
	int inodes = NUM_INODES;
	memset(buffer, 0, BLOCK_SIZE);
	memcpy(buffer, &magic, 4);
	memcpy(buffer + 4, &blocks, 4);
	memcpy(buffer + 8, &inodes, 4);
	writeBlock(disk, 0,0, buffer);

	//FREE VECTOR BLOCK
	memset(buffer, 1, BLOCK_SIZE);
	memset(buffer, 0, 10);
	writeBlock(disk, 1,0, buffer);
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
	writeBlock(disk, blockNum, offset, buff);
	fclose(disk);
}

//creates a file with given file name
//Returns 0 for succesfull, 1 for error
int createFile(char *filename){
	findOpenBlock(0);
		

	return(0);
}

int writeToFile(char *filePath, char *contents){
	




}

int createDir(char *dirName){
	findOpenBlock(0);
	int entries[16];

	return(0);
}

//returns an int to an available block
//returns value of open block (>10)
//if no block found returns 1
int findOpenBlock(int fileType){

	char *buffer;
	int i = 0;
	buffer = (char *)malloc(BLOCK_SIZE);
	readB(1, buffer);
	if(fileType == 0){//if file type is flat file
		while(buffer[i] != 1){
			if( i == BLOCK_SIZE){
				printf("no open block found\n");
				return(1);
			}
			i++;
		}
	}else{//if looking for a block for an inode
		i = NUM_BLOCKS;
		while(buffer[i] != 1){
			if( i == BLOCK_SIZE-128){
				printf("no open block found\n");
				return(1);
			}
			i--;
		}
	}


	free(buffer);
	markBlockTaken(i);
	return (i);
}

int createInode(int fileSize, int fileType){
	char *buffer = malloc(32);
	memset(buffer, 0, 32);
	memcpy(buffer, &fileSize, 4);
	memcpy(buffer + 4, &fileType, 4);
	int blockNum = findOpenBlock(fileType);
	writeB(blockNum, 0, buffer);
	free(buffer);
	return(0);	

}

void markBlockTaken(int blockNum){
	char *buffer;
	buffer = (char *)malloc(BLOCK_SIZE);
	readB(1, buffer);
	buffer[blockNum] = 0;
	writeB(1, 0, buffer);
	free(buffer);	

}











