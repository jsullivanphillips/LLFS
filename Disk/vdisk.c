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
const char DISKNAME[12] = "virtualDisk";
void readBlock(FILE* disk, int blockNum, char* buffer){
	fseek(disk, blockNum * BLOCK_SIZE, SEEK_SET);
	fread(buffer, BLOCK_SIZE, 1, disk);
}

void writeBlock(FILE* disk, int blockNum, char* data){
	fseek(disk, blockNum * BLOCK_SIZE, SEEK_SET);
	fwrite(data, BLOCK_SIZE, 1, disk); //will overwrite exisitng data
}

int main (int argc, char *argv[]){
	int ret, i, n;
	int fd;
	char buf[BLOCK_SIZE];
	
	
	ret = open (DISKNAME, O_CREAT | O_RDWR, 0666);
	if( ret == -1){
		printf("could not create disk\n");
		exit(1);
	}
	bzero((void*)buf, BLOCK_SIZE);

	fd = open(DISKNAME, O_RDWR);
	for(i = 0; i < NUM_BLOCKS; i++){
		printf("block=%d\n", i);
		n = write(fd, buf, BLOCK_SIZE);
		if(n != BLOCK_SIZE){
			printf("write error\n");
			exit(1);
		}
	}

	close(fd);

	printf("created virtual disk= %s of size= %d\n", DISKNAME, BLOCK_SIZE*NUM_BLOCKS);

	return(0);
}
