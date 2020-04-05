#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

void initLLFS();

void createDisk();

void formatDisk();

unsigned char create_inode(int blockNum);

void pushLog();

void writeLog(int blockNum, unsigned char *buffer);

void clean();

int findOpenBlock();

void updateFreeBlock();

#endif
