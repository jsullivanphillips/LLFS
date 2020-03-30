#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

void initLLFS();

void formatDisk();

void readB(int blockNum, char *buff);

void writeB(int blockNum, char *buff);

struct inode;

int createFile(char *filename);

int findOpenBlock();

void markBlockTaken(int blockNum);
#endif
