#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

void initLLFS();

void createDisk();

void formatDisk();

int findOpenBlock();

void updateFreeBlock();

#endif
