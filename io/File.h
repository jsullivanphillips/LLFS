#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

void initLLFS();

void formatDisk();

void readB(int blockNum, char *buff);

void writeB(int blockNum, int offset, char *buff);

struct inode;

int createFile(char *filename);

int createDir(char *dirName);

int findOpenBlock(int fileType);

int createInode(int fileSize, int fileType, int blockNum);

int writeToFile(char *filePath, char *contents);

void markBlockTaken(int blockNum);
#endif
