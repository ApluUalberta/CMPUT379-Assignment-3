#include <stdio.h>
#include <stdint.h>
#include "FileSystem.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include <vector>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <cstring>
#include <fcntl.h>
#include <vector>
#include <sys/types.h>
#include <unistd.h>




#define SOH "\x01"
#define INODES 126

void fs_write(char name[5], int block_num);
void fs_buff(uint8_t buff[1024]);
void fs_ls(void);
void fs_resize(char name[5], int new_size);
void fs_defrag(void);
void fs_cd(char name[5]);



Super_block Disk;
ssize_t size1 = 1024;
std:: unordered_map <uint8_t, std::unordered_set<std::string>> direcmap;

std:: vector<std::string> not_used;
std:: vector<std:: string> used;


int isBitISet( uint8_t ch, int i ){
    uint8_t mask = 1 << i;
    return mask & ch;
} 

void fs_mount(char *new_disk_name){
    // check if the file name exists within the current working directory
    int ConsistencyError = 0;

    int filed = ::open(new_disk_name,O_RDWR);
    if (filed > 0)
        std:: cout << "unable to open disk\n";
    //if found..

    ::read(filed, Disk.free_block_list, 16);

    for (int i = 0; i < INODES; i++){
        Inode new_inode;

        ::read(filed,&(new_inode.name),5);
        ::read(filed,&(new_inode.used_size),1);
        ::read(filed,&(new_inode.start_block),1);
        ::read(filed,&(new_inode.dir_parent),1);

        //put the new inode into the data structure
        Disk.inode[i] = new_inode;

        if (!isBitISet(new_inode.used_size,7)){
            not_used[i] = (std:: string) new_inode.name;
        }
        else if (isBitISet(new_inode.used_size, 7)){
            used[i] = (std:: string) new_inode.name;
        }
    }
    
    // Blocks that are marked free in the free-space list cannot be allocated to any file. Similarly, blocks marked in use in the free-space list must be allocated to exactly one file.

    // check if the blocks that are marked free are unallocated..



    for (size_t i = 0; i < INODES; ++i){
        uint8_t parent = Disk.inode[i].dir_parent & 0x7F;
        std::string name = std::string(Disk.inode[i].name).substr(0, 5);

        if (direcmap.find(parent) != direcmap.end() && direcmap[parent].find(name) != direcmap[parent].end()){
            ConsistencyError = 2;
            break;
        }
        else if (!name.empty()){
            direcmap[parent].insert(name);
        }
    }
    

    //blocks that are unmarked (free if in the free space list cannot be allocated to any file currently). Otherwise, the block is allocated to exactly one file.
    // Check if the unmarked blocks have anything assigned to them.
    


    // the true indicator of a free inode is if all bits are 0. In other words, check if the inode has any 1's.
    // set the root to the current working directory

    // set the name of the file system to new_disk_name

}

void fs_create(char name[5], int size){
    // Inode new_file = new Inode;

    // new_file->name = name;
    // new_file.size = (uint8_t) size;
    
    //store the given file in the directory

}

void fs_delete(char name[5]){
    // deletes the specified file in the current working directory

    //get cwd

    // search for the name that matches with the current name.

    // delete the file or directory with char name[5]


}

void fs_read(char name[5], int block_num){

}



void fs_write(char name[5], int block_num){

}




void tokenize(char* str, const char* delim, char ** argv) {
    char* token;
    token = strtok(str, delim);
    for(size_t i = 0; token != NULL; ++i){
        argv[i] = token;
        token = strtok(NULL, delim);
    }
}

int main(){

    std:: string input;
    std:: vector<std:: string> string_input_list;

    getline (std::cin, input);

}


