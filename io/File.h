#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

void initLLFS();

void createDisk();

void formatDisk();

int create_inode(int blockNum);

void makeDir(char *dirName);

void addToCurrDir(char *dirName, int inode_val);

void openFile(char *fileName);

void write_file(char *contents);

void get_cur_file_size();

void get_cur_file_blocks();

void cd(char* dirName);


void updateDir(char *fileName, int blockNum);

int update_inode(int cur_file_inode, int blocks[], int size_of_contents);

int existsInDir(char *fileName);

void markBlockFree(int blockNum);

int spaceInCurDir();

void pushLog();

void writeLog(int blockNum, char *buffer);

void cd(char *dir);

void clean();

int findOpenBlock();

void updateFreeBlock();

#endif
