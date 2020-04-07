# LLFS
CSC360 Assignment3

implemented features:
openfile
write file
make dir
change directory



change directory does not tokenize, attempted to do it but it only made the program crash.
Ran out of time! should started earlier!



Log structure file system. 
Used a local variable to track obselete blocks for easier garbage collection once disk becomes full. This uses up more local
memory but allows for a simpler garbage collection then tracing pointers to empty obsolete blocks.


used https://www.youtube.com/watch?v=KTCkW_6zz2k

instead of using an imap i am tracking the locations of the inodes for all files and directorys within the directories themselves. 
I am choosing to do this because the imap is unnecessary for the scope of this assignment, and would require excess work to implement.

