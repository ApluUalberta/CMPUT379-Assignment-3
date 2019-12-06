CMPUT 379 Assignment 3 README

Design choices
------------------------------
The Design choices for the FileSystem involved a lot of repetition throughout each function carried over from other functions. Therefore, fs_mount's consistency checks had a lot of repetitive code for each consistency check, but with a minor tweak to ensure that the checks happened correctly, and in order. Additionally, updating the disk ended up being copy/pasted for every function that required a disk update with any given procedure. Also, an externally sourced function used to check if a bit was set was used to check if certain bits of an Inode were marked throughout the program.



System calls for each functions
------------------------------


