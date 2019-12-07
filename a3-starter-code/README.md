CMPUT 379 Assignment 3 README

Design choices
------------------------------
The Design choices for the FileSystem involved a lot of repetition throughout each function carried over from other functions. Therefore, fs_mount's consistency checks had a lot of repetitive code for each consistency check, but with a minor tweak to ensure that the checks happened correctly, and in order. Additionally, updating the disk ended up being copy/pasted for every function that required a disk update with any given procedure. Also, an externally sourced function used to check if a bit was set was used to check if certain bits of an Inode were marked throughout the program.



System calls for each functions
------------------------------
fs_mount():
- read() system call was used to read the bytes in the file descriptor which was used to read the from the mounted disk.
- memcpy() was used to copy the size to the address of the start of the block to get proper positioning 
- lseek() was used to reset the file pointer in order to proceed with 6 checks accordingly.


fs_create():
- memset() syscall was used to reset the inode's name 
- memcpy() was used to copy the new name into the inode's name space with a limit of 5 characters.
- lseek() was used to reset the file pointer to point to the contents of the name, parent, usedsize, and startblock.
- write() was used to update the created attributes on the disk to its given blocks.
- open() was used to open the file/directory to delete in order to update the disk accordingly.

fs_delete():
- memset() was used to empty out the inode pertaining to the file to be deleted.
- open() was used to open the file/directory to delete in order to update the disk accordingly.
- lseek() was used to reset the file pointer to point to the contents of the name, parent, usedsize, and startblock.

fs_read():
- lseek() was used to start at the contents of the memory block to read.
- read() was used to access the lseek() data and read the contents of the desired file to read.
- close() was used to terminate the file pointer after reading.

fs_cd():
- no system calls used. only strcmp library funstions.

fs_write():
- lseek() was used to start at the contents of the memory block to write to.
- read() was used to access the lseek() data and read the contents of the desired block to write to.
- close() was used to terminate the file pointer after writing.

fs_buff():
- memset() was used to clear out the global buffer
- memcpy() was used to copy the passed in buffer tothe global buffer in order to update the global buffer.

fs_ls():
- memset(name,0,6) was used to clear out an inode's name before being copied into.

readInputs():
- memset() was used to clear out buffers, and name arrays to be updated.

fs_defrag();
- defrag was not implemented

fs_resize():
- resize was not implemented

Testing
---------------------------------
simple input and output comparisons with hand-made input files were made and then traced to figure out the proper output by hand to be checked against the fs_ls() output in the terminal. hex_dump was then also used to check if values were being updated within the disk properly. Since not every function was implemented, test cases needed to be generated alone with hand-made outcomes, such as creating and deleting files and directories, or directories with children (which could later be deleted).
- Testing create was done by creating a file and directory
- Testing delete was done by deleting a directory, file, and directory with children for proper recursion
- Testing read/write was done by running sample test 1 and using hex dump to look at the updated disk compared to the result disk
- Testing buffer was done by printing the values in the buffer that was copying and the buffer that was copied too.
- Testing CD was used by piggybacking off of ls to see if the current directory's number of children was proper.
- Testing mount was done by using sample consistency checks for each given trap case which were then returned.
- Testing main was done by printing tokenized strings to see if the right values were being passed in.

Sources
-------------------------------
Lab 9 Slides
https://eclass.srv.ualberta.ca/pluginfile.php/5460007/mod_resource/content/1/379%20Lab%209.pdf

Assignment 3 PDF
https://eclass.srv.ualberta.ca/pluginfile.php/5430525/mod_resource/content/1/Assignment_3.pdf

Dragonshell startercode (tokenize function)
https://eclass.srv.ualberta.ca/mod/assign/view.php?id=3770413

Bit checker Tutorial: (isbitiset function)
https://eclass.srv.ualberta.ca/pluginfile.php/5460123/mod_resource/content/1/Bits.pdf