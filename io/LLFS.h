

void createFile(){
	//check to see if there are any free inode blocks
	//if there is create new inode with name, type of file (dir or data), etc..
	//check if there are any free data blocks, if so then allocate memory for new file
	//create a new directory entry for the new file in its given directory
	//
	//if at any step this cant be done, return an error 
}

void deleteFile(){
	//find the inode given directory listing and name
	//get the location of the files blocks from the inode
	//check if file is directory, if it is, check if it is empty
	//Deallocate the blocks listed in the inode, set them as free in the block vector
	//remove file listing from directory
	//deallocate inode and set its location as free
}

void writeFile(){
}

void readFile(){ //load a file from disk into memory
}

void findInode(){
}

void readInode(){
}

void modifyInode(){
}

void deleteInode(){
}

void findAvailableBlock(){
}

void formatDisk(){
}

