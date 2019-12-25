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
#include <map>
#include <set>


#define BUFFER_SIZE_PRESET 1024

#define MAX127 127
#define MAX126 126
#define MIN1 1
#define SOH "\x01"
#define INODES 126
#define USED_BLOCK 5
#define NAME_SIZE_LIMIT 6



Super_block *Disk;
Super_block *mockBlock;
uint8_t Directorylocation = 0;
std:: string Dname;
uint8_t buffer[BUFFER_SIZE_PRESET];

std:: string dot = ".";
std:: string doubledot = "..";
int size_macro = 8;
char nullchar = '\0';
int mask0x80 = 0x80;

// check if bit i is set in the given 8 bit integer

int isBitISet( uint8_t ch, int i ){
    uint8_t mask = 1 << i;
    return mask & ch;
} 
// counts a directory's children
int Child_count(uint8_t dir){
    int count = 0;

    for (int i = 0; i < INODES; i++){
        if ((Disk->inode[i].dir_parent|0x7f) == dir){
            count++;
        }    
    }
    return count;
}

void fs_mount(char *new_disk_name){
    // check if the file name exists within the current working directory
    int filed = open(new_disk_name,O_RDWR);
    if (filed < 0){
        std:: cerr << "unable to open disk\n";
	    return;
    }    //if found..

    // read the mock freeblock list
    Super_block *mockBlock = new Super_block;
    read(filed, mockBlock->free_block_list, 2*size_macro);

    //use a map to keep track of bytes
	std:: map<int,int> map1;
	for (int i = 0; i < INODES;i++){
        //create a temporary char array for consistency tests.
		char storage[size_macro];

        //reference the proper inode attributes in the correct positions
		read(filed,storage,size_macro);
		char Bused = storage[USED_BLOCK];
        uint8_t size = Bused & 0x7F;
        int intsizeversion = unsigned(size);
		uint8_t start;
        // copy the size to the address of the start of the block to get proper positioning 
        memcpy(&start, storage+6,sizeof(uint8_t));
		// check if the inode is in use
		if (isBitISet(Bused,7)){
            //loop to check from the left end
            for (int j = 0; j < intsizeversion; j++){
                //finding the location in the free block space that corresponds to the given bit
                uint8_t bytecheckmod = start + j;
                uint8_t bitcheckmod = start + j;
                int check_byte = (bytecheckmod)/size_macro;
                int check_bit = (bitcheckmod)%size_macro;
                check_bit = 7-check_bit;

                //if the buyte goes over the limit..
                if (check_byte > 2*size_macro){
                    continue;
                }
                // if the blocks that are marked free in the free-space list are allocated to any file.
                if (!isBitISet(mockBlock->free_block_list[check_byte],check_bit)){
                    std:: cerr << "Error: file system in " << new_disk_name << " is inconsistent with error code 1\n";
                    return;
                }
                if (map1.find(bytecheckmod)==map1.end()){
                    map1[start+j] = i;
                }else{ // if we are not at the end.
                    //error message.
                    std:: cerr << "Error: file system in " << new_disk_name << " is inconsistent with error code 1\n";
                    return;
                }
            }

		} 
        
	}
    //similarly, blocks marked in use in the free-space list must be allocated to exactly one file.
    for (int i = 0; i < 2*size_macro; i++){
        for (int j = 0; j < size_macro; j++){
            if (i == 0 && j == 0){
                continue;
            }
            if (isBitISet(mockBlock->free_block_list[i],7-j)){
                int arithmetic = size_macro*i+j;
                if (map1.find(arithmetic) == map1.end()){

                    std:: cout << "Error: file system in " << new_disk_name << " is inconsistent with error code 1\n";
                    return;
                }
            }
        }
    }      

    // reset the file pointer.
    int lseekval = -1*lseek(filed,0,SEEK_CUR);
    lseek(filed,lseekval,SEEK_CUR);
    lseek(filed,2*size_macro,SEEK_CUR);
    // read the mock freeblock list
	std:: multimap<std:: string,uint8_t> usedBlocks_2;
	for (int i = 0; i < INODES;i++){
		char storage[size_macro];
        //set the char array's elements to the corresponding inode placements.
		read(filed,storage,size_macro);
        char name[NAME_SIZE_LIMIT];
        memcpy(name,storage,USED_BLOCK);
        name[NAME_SIZE_LIMIT] = nullchar;
		char Sused = storage[USED_BLOCK];

		uint8_t start;
        // copy the size over to start to gauge properly.
        memcpy(&start,storage+6,sizeof(uint8_t));
		uint8_t pa;
        memcpy(&pa,storage + 7,sizeof(uint8_t));

        // check 7th bit
        int sev = 7;
		if (isBitISet(Sused,sev)){
            auto range = usedBlocks_2.equal_range((std::string) name);
            if (isBitISet(Sused,7)){
                std:: string nameString = (std:: string) name;
                // iterate through the map. If the parent is equal to the second argument.. the name is not unique
                for (auto i = range.first;i!= range.second;i++){

                    if (i->second==pa){
                        std:: cerr << "Error: file system in " << new_disk_name << "is inconsistent with error code 2\n";
                        return;
                    }
                }
                usedBlocks_2.insert(std::pair<std:: string,uint8_t>(nameString,pa));

            }

        }
    }


    // reset the file pointer.
    // check 3
    lseekval = -1*lseek(filed,0,SEEK_CUR);
    lseek(filed,lseekval,SEEK_CUR);
    lseek(filed,2*size_macro,SEEK_CUR);
    // read the mock freeblock list
	std:: multimap<std:: string,uint8_t> usedBlocks_3;
	for (int i = 0; i < INODES;i++){
		char storage[size_macro];
        //set the char array's elements to the corresponding inode placements.
		read(filed,storage,size_macro);
        char name[NAME_SIZE_LIMIT];
        memcpy(name,storage,USED_BLOCK);
        name[NAME_SIZE_LIMIT] = nullchar;
		char used = storage[USED_BLOCK];
		uint8_t start;
        // copy the size over to start to gauge properly.
        memcpy(&start,storage+6,sizeof(uint8_t));
		uint8_t parent;
        memcpy(&parent,storage+7,sizeof(uint8_t));
        int sev = 7;
        // check 7th bit
        if (isBitISet(used,sev)){
            //check if Otherwise, the name attribute stored in the inode must have at least one bit that is not zero.
            if ((std:: string) name == ""){
                std:: cerr << "Error: file system in " << new_disk_name << "is inconsistend with error code 3\n";
                return;
            }

        }
        // if the . If the state of an inode is free, check if the bits are all 0
        else{
            for (int i = 0; i < size_macro; i++){
                for (int j = 0; j < size_macro; j++){
                    if (isBitISet(storage[i],j)){
                        std:: cerr << "Error: file system in " << new_disk_name << " is inconsistent with Error 3\n";
                        return;
                    }
                }
            }
        }
    }
    // reset the file pointer.
    // check 4
    lseekval = -1*lseek(filed,0,SEEK_CUR);
    lseek(filed,lseekval,SEEK_CUR);
    lseek(filed,2*size_macro,SEEK_CUR);
    // read the mock freeblock list
	std:: multimap<std:: string,uint8_t> usedBlocks_4;
	for (int i = 0; i < INODES;i++){
		char storage[size_macro];
        //set the char array's elements to the corresponding inode placements.
		read(filed,storage,size_macro);
        char name[NAME_SIZE_LIMIT];
        memcpy(name,storage,USED_BLOCK);
        name[NAME_SIZE_LIMIT] = nullchar;
		char used = storage[USED_BLOCK];
		uint8_t start;
        uint8_t parent;
        // copy the size over to start to gauge properly.
        memcpy(&start,storage+6,sizeof(uint8_t));
        memcpy(&parent,storage+7,sizeof(uint8_t));
        // check 7th bit
        int sev = 7;
        if (isBitISet(used,sev)){
            // if in use, check if it is a directory or file
            if (!(isBitISet(parent,sev))){
                int sInt = (unsigned int) start;
                //check if inode is in range.
                if (sInt > MAX127 ||sInt <MIN1){
                    std:: cerr << "Error, File System in " << new_disk_name << " is inconsistent with Error 4\n";
                    return;
                }
            }
        }
    }
    // reset the file pointer.
    // check 5
    int lseeksecond = -1*lseek(filed,0,SEEK_CUR);
    lseek(filed,lseeksecond,SEEK_CUR);
    lseek(filed,2*size_macro,SEEK_CUR);
    // read the mock freeblock list
	std:: multimap<std:: string,uint8_t> usedBlocks_5;
	for (int i = 0; i < INODES;i++){
		char storage[size_macro];
        //set the char array's elements to the corresponding inode placements.
		read(filed,storage,size_macro);
        char name[NAME_SIZE_LIMIT];
        memcpy(name,storage,USED_BLOCK);
        name[NAME_SIZE_LIMIT] = nullchar;
		char used = storage[USED_BLOCK];
        char size = used & 0x7f;
        int sizeInt = (int) size;
        // copy the size over to start the gauge properly
		uint8_t start;
        uint8_t par;
        memcpy(&start,storage+6,sizeof(uint8_t));
        memcpy(&par,storage+7,sizeof(uint8_t));
        //check 7th bit
        int sev = 7;
        if (isBitISet(used,sev)){
            if (isBitISet(par,sev)){
                //check if the size and start block of the directory is 0
                if ((unsigned) sizeInt != 0 || (unsigned)start != 0){
                    std:: cerr<< "Error: File system in " << new_disk_name << " is inconsistent with Error 5\n";
                    return;
                }
            }
        }
    }
    //reset the file pointer
    int lseekthird = -1*lseek(filed,0,SEEK_CUR);
    // check 6
    lseek(filed,lseekthird,SEEK_CUR);
    lseek(filed,2*size_macro,SEEK_CUR);
    // read the mock freeblock list
	std:: set<int> usedBlocks_6;
	for (int i = 0; i < INODES;i++){
		char storage[size_macro];
        //set the char array's elements to the corresponding inode placements.
		read(filed,storage,size_macro);
        char name[NAME_SIZE_LIMIT];
        memcpy(name,storage,USED_BLOCK);
        name[NAME_SIZE_LIMIT] = nullchar;
		char used = storage[USED_BLOCK];
        // copy the size over to start the gauge properly

		uint8_t start;
        memcpy(&start,storage+6,sizeof(uint8_t));
		uint8_t par;
        memcpy(&par,storage+7,sizeof(uint8_t));
        //check 7th bit
        par = par & 0x7F;
        if (isBitISet(used,7)){
            //For every inode, the index of its parent inode cannot be 126.
            if (par == MAX126){
                std:: cerr << "Error: File System in " << new_disk_name << " is inconsistent with Error 6\n";
                return;
            }
            if (par != MAX127){
                //std::cout<<"node "<<i<<" has parent "<<unsigned(par)<<"\n";
                usedBlocks_6.insert(par);
            }
        }


        for (int it : usedBlocks_6){
            // if the index of the parent inode is between 0 and 125 inclusive, then the parent inode must be in use and marked as a directory.

            int lseekfinal = -1*lseek(filed,0,SEEK_CUR);
            lseek(filed,lseekfinal,SEEK_CUR);
            lseek(filed,2*size_macro,SEEK_CUR);
            int arithmetic = size_macro*it;
            lseek(filed,arithmetic,SEEK_CUR);
            char storage[size_macro];
            read(filed,storage,size_macro);
            char used = storage[USED_BLOCK];
            uint8_t par;
            //check if the inode is free or allocated to a file.
            memcpy(&par,storage+7,sizeof(uint8_t));
            int sev = 7;
            if (!isBitISet(used,sev)||!isBitISet(par,sev)){
                std:: cerr<< "Error: File System in " << new_disk_name << " is inconsistent with error 6a\n";
                return;
            }
        }

    }



    //reset the pointer
    lseek(filed,0,SEEK_SET);
    //read the disk to then mount the disk
    read(filed,Disk->free_block_list,16);
    for (int i = 0; i < INODES; i++){
        //copy all the inodes over to mount the disk
        Inode newI;
        char storage[size_macro];
        read(filed,storage,size_macro);
        memcpy(newI.name, storage,USED_BLOCK);

        memcpy(&(newI.used_size),storage+5,1);
        //mask used with 0x7f
        newI.used_size &= 0x7f;
        memcpy(&(newI.start_block),storage+6,1);
        memcpy(&(newI.dir_parent),storage+7,1);
        // mask parent with 0x7f

        Disk->inode[i] = newI;
    }
    //update the name and directory as root..
    Directorylocation =(uint8_t) MAX127;
    Dname = (std:: string) new_disk_name;

}

void fs_create(char name[5], int size){
    //disk has not been mounted
    if (Dname.empty()){
        std::cout << "No disk is mounted\n";
        return;
    }
    int i = 0;
    //iterate over the inodes to check if there are any free inodes
    while (i < INODES){
        if (strcmp(Disk->inode[i].name,"") == 0){
            //inode that is not in use is found 
            break;
        }
        i = i + 1;
    }
    //if there are no free blocks in memory
    if (i == INODES){
        std:: cout <<  "Error: Superblock in disk "<< Dname << " is full, cannot create " << name << "\n";
        return;
    }
    //iterate over inodes to check if the directory already exists
    for (int j = 0; j < INODES; j++){
        if (strncmp(Disk->inode[j].name,name,USED_BLOCK) == 0){
            uint8_t parent = Disk->inode[i].dir_parent & 0x7f;
            if(parent == Directorylocation){
                std:: cout << "Error: File or directory " << name <<" already exists\n";
                return;
            } 
        }
    }
    //handle if user inputs ..
    if (strcmp(name,dot.c_str()) == 0){
        std:: cout <<"Error: File or directory " << name <<" already exists\n";
        return;
    }
    //handle if user inputs .
    if (strcmp(name,doubledot.c_str()) == 0){
        std:: cout <<"Error: File or directory " << name <<" already exists\n";
        return;
    }

    //create a new file within the current working directory
    uint8_t start;
    uint8_t dir;
    //check if the creation is of a directory or not
    if (size == 0){
        // create a directory
        dir = 1;
        dir = dir << 7;
        start = 0;
    }else{
        //is file creation
        dir = 0;
        int count = 0;
        int j = 0;
        int k = 7;
        // iterate to find the necessary amount of free space. if a non-free space is encountered, count resets and tries again until it reaches size.
        for (; j < 16; j++){

            k = 7;
            for (; k > -1; k--){
                if (isBitISet(Disk->free_block_list[j],k)){
                    count = 0;
                }else if (!isBitISet(Disk->free_block_list[j],k)){
                    count++;
                }
                if (count == size){
                    //found 5 free spaces
                    break;
                }
            }
            //found..
            if (count == size){
                break;
            }
        }
        // set the found blocks to 1's to mark in use...
        int blocknum =8*j+7-k;
        start = blocknum - size+1;
        //mark
        int mask = 1;
        while (count > 0){
            Disk->free_block_list[j] |= ((int)mask << k);
            count--;
            k++;
            if (k > 7){
                j--;
                k = 0;
            }
        }
    }
    //copy updated values into the inodes.
    Disk->inode[i].dir_parent = Directorylocation|dir;
    Disk->inode[i].start_block = start;
    Disk->inode[i].used_size = size|0x80;
    //copy name..
    memset(Disk->inode[i].name,0,5);
    memcpy(Disk->inode[i].name, name,5);

    //update the disk..
    int file_d = open(Dname.c_str(),O_RDWR);
    if (file_d < 0){
        std:: cerr << "unable to open disk\n";
	    return;
    }
    //write to the disk..
    write(file_d,Disk->free_block_list, 16);

    for (int z = 0; z < INODES; z++){
        Inode inode = Disk->inode[z];
        char *name = new char[5];
        //write the inode values to the disk..
        std:: memcpy(name,inode.name,5);
        int num = write(file_d,name,5);
        lseek(file_d,5-num,SEEK_CUR);
        write(file_d,&inode.used_size,1);
        write(file_d,&inode.start_block,1);
        write(file_d,&inode.dir_parent,1);
        delete[] name;

    }
    //terminate the file pointer.
    close(file_d);

}

void fs_delete(char name[5]){
    if (Dname.empty()){
        std::cout << "No disk is mounted\n";
        return;
    }
    //iterate through the inodes to check where the names match up
    for (int j = 0; j < INODES; j++){
        // if the names match up
        if (strncmp(Disk->inode[j].name,name,5) == 0){
            uint8_t parent = Disk->inode[j].dir_parent & 0x7f;
            // if the file is in the current directory
            if(parent == Directorylocation){
                //if directory
                if (Disk->inode[j].dir_parent&0x80){
                    
                    //recursively delete the children.
                    //call fs_delete() on map elements
                    std:: map<int, Inode> Dchildren;
                    for (int k = 0; k < INODES; k++){
                        //find all inodes with the parent j
                        if ((Disk->inode[k].dir_parent & 0x7f) == parent){
                            Dchildren.insert(std::pair<int,Inode>(k,Disk->inode[k]));
                        }
                    }
                    uint8_t savedDir = Directorylocation;
                    //update the directory location for fs_delete()
                    Directorylocation = j;
                    //fs_delete is called upon map elements....
                    for (auto itr = Dchildren.begin(); itr != Dchildren.end(); itr++){
                        fs_delete(itr->second.name);
                    }


                    Directorylocation = savedDir;

                    //update inodes.
                    memset(&Disk->inode[j].name,0,5);
                    memset(&Disk->inode[j].start_block,0,1);
                    memset(&Disk->inode[j].used_size,0,1);
                    memset(&Disk->inode[j].dir_parent,0,1);

                    // update the disk..

                    int file_d = open(Dname.c_str(),O_RDWR);
                    if (file_d < 0){
                        std:: cerr << "unable to open disk\n";
                        return;
                    }

                    lseek(file_d,0,SEEK_SET);
                    write(file_d,Disk->free_block_list, 16);

                    for (int z = 0; z < INODES; z++){
                        Inode inode = Disk->inode[z];
                        char *name = new char[5];
                        std:: memcpy(name,inode.name,5);
                        int num = write(file_d,name,5);
                        //write to the disk.
                        lseek(file_d,5-num,SEEK_CUR);
                        write(file_d,&inode.used_size,1);
                        write(file_d,&inode.start_block,1);
                        write(file_d,&inode.dir_parent,1);
                        delete[] name;

                    }
                    close(file_d);


                }else{
                    //file deletion.
                    int start = Disk->inode[j].start_block;
                    int bytecheck = start/8;
                    int bitcheck = start%8;
                    bitcheck = 7 - bitcheck;
                    // go through the proper blocks in memory fro the free block list to mark as 0
                    int numberofbits = 0;
                    while (numberofbits < (Disk->inode[j].used_size &0x7f)){
                        
                        int mask = 1 << bitcheck;
                        Disk->free_block_list[bytecheck] &= ~mask;
                        numberofbits++;
                        bitcheck--;
                        if (bitcheck <0){
                            bitcheck = 7;
                            bytecheck++;
                        }
                    }
                    //update the inode values.
                    memset(&Disk->inode[j].name,0,5);
                    memset(&Disk->inode[j].start_block,0,1);
                    memset(&Disk->inode[j].used_size,0,1);
                    memset(&Disk->inode[j].dir_parent,0,1);


                    //update the disk
                    int filed = open(Dname.c_str(),O_RDWR);
                    lseek(filed,Disk->inode[j].start_block*1024,SEEK_SET);
                    for (int p = 0; p < INODES; p++){
                        write(filed,0,1024);
                    }

                    lseek(filed,0,SEEK_SET);
                    write(filed,Disk->free_block_list, 16);

                    for (int z = 0; z < INODES; z++){
                        Inode inode = Disk->inode[z];
                        char *name = new char[5];
                        std:: memcpy(name,inode.name,5);
                        int num = write(filed,name,5);
                        lseek(filed,5-num,SEEK_CUR);
                        //write to the disk.
                        write(filed,&inode.used_size,1);
                        write(filed,&inode.start_block,1);
                        write(filed,&inode.dir_parent,1);
                        delete[] name;

                    }
                    close(filed);
                }
            } 
        }
    }

}

void fs_read(char name[5], int block_num){
    //check if the disk is mounted.
    if (Dname.empty()){
        std::cout << "No disk is mounted\n";
        return;
    }

    //iterate over the inodes
    for (int i = 0; i < INODES; i++){
        //get the parent of the inode
        uint8_t parent = Disk->inode[i].dir_parent & 0x7f;
        //check if the name is matching
        if (strncmp(Disk->inode[i].name,name,5) == 0){
            //if we are in the right directory
            if (parent == Directorylocation){
                //read the disk
                int filed = open(Dname.c_str(),O_RDWR);
                lseek(filed, 1024*(Disk->inode[i].start_block)+1024*block_num, SEEK_SET);
                read(filed,buffer,1024);
                close(filed);
                return;
            }
            else{
                std:: cerr << "Error: " << name << "does not have block " << block_num << "\n";
                return;
            }
        }
    }
    std::cerr << "Error: file " << name << "does not exist\n";
    return;
}


void fs_cd(char name[5]){
    // changes the given directory to the specified directory
    if (Dname.empty()){
        return;
    }
    //is cd going to parent?
    if (strcmp(name,"..") == 0){
        if (Directorylocation != MAX127){
            Directorylocation = Disk->inode[Directorylocation].dir_parent & 0x7f;
        } 
        return;
    }
    // is cd just the current directory?
    if (strcmp(name, ".") == 0){
        return;
    }
    //else..
    for (uint8_t i = 0; i < INODES;i++){
        //check the parent value;..
        uint8_t parent = Disk->inode[i].dir_parent;
        parent = parent & 0x7F;
        //if the name matches and we are in the right directory
        if (strncmp(Disk->inode[i].name,name,5) == 0 && (parent == Directorylocation)){
            // we change the given direcotry's location.
            if ((Disk->inode[i].dir_parent & 0x80)){
                Directorylocation = i;
                return;
            }
        }
    }
    std:: cerr << "Error, File/directory " << name << " does not exist\n";
}

void fs_write(char name[5], int block_num){
    // check if the disk is mounted..
    if (Dname.empty()){
        std::cout << "No disk is mounted\n";
        return;
    }
    //iterate over the inodes
    for (int i = 0; i < INODES; i++){
        //retrieve the dir parent 
        uint8_t parent = Disk->inode[i].dir_parent & 0x7f;
        //string match the names
        if (strncmp(Disk->inode[i].name,name,5) == 0){
            // if we are in the proper directory..
            if (parent == Directorylocation){
                //write to the disk..
                int filed = open(Dname.c_str(),O_RDWR);
                lseek(filed, 1024*(Disk->inode[i].start_block)+1024*block_num, SEEK_SET);
                write(filed,buffer,1024);
                close(filed);
                return;
            }
            //write buffer to the block_numth block of the file...
            // take the buffer
            else{
                std:: cerr << "Error: " << name << "does not have block " << block_num << "\n";
                return;
            }
        }
    }
    std::cerr << "Error: file " << name << "does not exist\n";
    return;
}

void fs_buff(uint8_t buff[1024]){
    // check if the disk is mounted..
    if (Dname.empty()){
        std::cout << "No disk is mounted\n";
        return;
    }
    //clear the buffer
    memset(buffer,0,sizeof(buffer));
    //rewrite the buffer.
    memcpy(buffer,buff,sizeof(buffer));
}
void fs_defrag(void){
    // check if the disk is mounted..
    if (Dname.empty()){
        std::cout << "No disk is mounted\n";
        return;
    }
}

void fs_resize(char name[5], int new_size){
    // check if the disk is mounted..
    if (Dname.empty()){
        std::cout << "No disk is mounted\n";
        return;
    }
}

void fs_ls(void){
    // check if the disk is mounted..
    if (Dname.empty()){
        std::cout << "No disk is mounted\n";
        return;
    }
    //insert the children of the current directory into a map of inodes along with its index.
    std:: map <int,Inode> mymap;
    for (int i = 0; i < INODES; i++){
        if (Directorylocation == (Disk->inode[i].dir_parent & 0x7f)){
            mymap.insert(std::pair<int,Inode>(i,Disk->inode[i]));
        }
    }
    // print the current directory and the amount of children it has
    printf(".     %3d\n", (int)mymap.size()+2);

    // if root
    if (Directorylocation == 127){
        //print the root directory size
        printf("..    %3d\n", (int)mymap.size()+2);  
    }else{
        //print the directory and its number of children.
        printf("..    %3d\n", Child_count((Disk->inode[Directorylocation].dir_parent &0x7f))+2);
    }
    //iterate through the map to find all of the children to list.
    for (auto it = mymap.begin(); it != mymap.end(); it++){
        //retrieve the names of the children
        char * name = new char[6];
        memset(name,0,6);
        strncpy(name, it->second.name,5);
        name[6] = '\0';
        

        // check if the inode is in use
        if (it->second.used_size & 0x80){
            // check if file
            if (!(it->second.dir_parent & 0x80)){
                //print file size
                printf("%-5s %3d KB\n", name, it->second.used_size & 0x7f);
            }else{
                // print directory size.
                printf("%-5s %3d\n", name, Child_count(Disk->inode[Directorylocation].dir_parent&0x7f)+2);
            }
        }
        delete[] name;
    }

}

std::vector<std::string> tokenize(const std::string &str, const char *delim) {
    //dragonshell's tokenize function..

    char* cstr = new char[str.size() + 1];
    std::strcpy(cstr, str.c_str());

    char* tokenized_string = strtok(cstr, delim);

    std::vector<std::string> tokens;
    while (tokenized_string != NULL)
    {
        tokens.push_back(std::string(tokenized_string));
        tokenized_string = strtok(NULL, delim);
    }
    delete[] cstr;

    return tokens;
}


int readInput(std:: string command){
    //parsing commands properly
    std:: vector<std:: string> tokenizer = tokenize(command.c_str()," ");
    // fs mount..
    if (tokenizer[0] == "M" && tokenizer.size() == 2){
        //pass in the disk name to check and mount....
        char *arr = new char[tokenizer[1].size()+1];
        strcpy(arr, tokenizer[1].c_str());
        fs_mount(arr);
        delete[] arr;
        return 0;
    } // fs create
    else if (tokenizer[0] == "C" && tokenizer.size() == 3){
        //if the name is over 5 chars in length.
        if (tokenizer[1].size() > USED_BLOCK){
            std:: cerr << "Command Error: <input file name>, <line number>";
            return -1;
        } // if the size is out of range.
        if (stoi(tokenizer[2]) > 127 || stoi(tokenizer[2]) < 0){
            return -1;
        }
        // copy the name and pass to fs create along with the third arg as its size.
        char *arr = new char[6];
        memset(arr,0,6);
        strcpy(arr,tokenizer[1].c_str());
        int size = stoi(tokenizer[2]);
        fs_create(arr,size);
        delete[] arr;
        return 0;
    }
    else if (tokenizer[0] == "D" && tokenizer.size() == 2){
        // copy the name to delete..
        char *arr = new char[6];
        memset(arr,0,6);
        strcpy(arr,tokenizer[1].c_str());
        fs_delete(arr);
        delete[] arr;
        return 0;
    }
    else if (tokenizer[0] == "R" && tokenizer.size() == 3){
        //copy the name to read.
        char *arr = new char[tokenizer[1].size() + 1];
        strcpy(arr, tokenizer[1].c_str());
        int bnumber = stoi(tokenizer[2]);
        fs_read(arr, bnumber);
        delete[] arr;
        return 0;
    }
    else if (tokenizer[0] == "W" && tokenizer.size() == 3){
        // pass in a copied string of the arguments to write
        char *arr = new char[tokenizer[1].size() + 1];
        strcpy(arr, tokenizer[1].c_str());
        int bnumber = stoi(tokenizer[2]);
        fs_write(arr, bnumber);
        delete[] arr;
        return 0;
    }
    else if (tokenizer[0] == "B" && tokenizer.size() >= 2){
        if ((command.size() - command.find("B")> 1026)){
            // command exceeds the size of the buffer
            std:: cerr << "command exceeds the size of the buffer.\n";
            return -1;
        }
        //pass in a buffer to update the existing one.
        uint8_t *buff = new uint8_t[BUFFER_SIZE_PRESET];
        memset(buff,0,1024);
        copy(command.begin() + command.find("B"),command.end(), buff);
        fs_buff(buff);
        delete[] buff;
        return 0;
    }
    else if (tokenizer[0] == "L" && tokenizer.size() == 1){
        //call ls to see the children of the current directory
        fs_ls();
        return 0;
    }
    else if (tokenizer[0] == "E" && tokenizer.size() == 3){

        //pass in the copied tokenized string to fs_resize.
        char *arr = new char[tokenizer[1].size() + 1];
        strcpy(arr, tokenizer[1].c_str());
        int bnumber = stoi(tokenizer[2]);
        fs_resize(arr,bnumber);
        delete[] arr;
        return 0;

    }
    else if (tokenizer[0] == "O" && tokenizer.size() == 1){
        //defrag..
        fs_defrag();
        return 0;
    }
    else if (tokenizer[0] == "Y" && tokenizer.size() == 2){
        //pass the name of the file/directory to go into.
        char *arr = new char[6];
        memset(arr,0,6);
        strcpy(arr,tokenizer[1].c_str());
        arr[6]='\0';
        // go into a given directory.
        fs_cd(arr);
        delete[] arr;
        return 0;
    }
    return -1;

}

int main(int argc, char *argv[]){
    if (argc != 2){
        std:: cerr << "Usage: ./fs <input_file>\n";
    }
    //clear the given buffer...
    memset(buffer,0,sizeof(buffer));
    std:: string line;
    //retrieve the commands.
    char * input = argv[1];
    std:: ifstream commands (input);
    //initialize the superblock objects to read and write to a disk.
    Disk = new Super_block;
    mockBlock = new Super_block;
    // if commands are invalid.
    if (!commands.is_open()){
        return -1;
    }else{ //if the command is valid, we keep getting input for any input file.
        int lnumber = 1;
        while (getline(commands,line)){
            if (readInput(line)==-1){
                std:: cerr << "Command Error: " << input << ", " << lnumber << "\n";
            }
            lnumber = lnumber + 1;
        }
    }
    //clear memory.
    delete Disk;
    delete mockBlock;
    return 0;
}
