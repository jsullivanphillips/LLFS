#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

void initLLFS();

void formatDisk();

void readB(int blockNum, char *buff);

void writeB(int blockNum, int offset, char *buff);

int findOpenBlock();

void markBlockTaken(int blockNum);

void printFreeBlock();

#endif
