#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

void initLLFS();

void createDisk();

void formatDisk();

int create_inode(int blockNum);

void makeDir(char *dirName);

void addToCurrDir(char *dirName, int inode_val);

void markBlockFree(int blockNum);

int spaceInCurDir();

void pushLog();

void writeLog(int blockNum, char *buffer);

void cd(char *dir);

void clean();

int findOpenBlock();

void updateFreeBlock();

#endif
