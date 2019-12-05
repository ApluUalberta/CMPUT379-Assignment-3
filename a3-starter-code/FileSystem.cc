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

#define MAX127 127
#define MAX126 126
#define SOH "\x01"
#define INODES 126

Super_block *Disk;
Super_block *mockBlock;
uint8_t Directorylocation = 0;
std:: string Dname;
uint8_t buffer[1024];

int mask = 0x7f;

int isBitISet( uint8_t ch, int i ){
    uint8_t mask = 1 << i;
    return mask & ch;
} 

void fs_mount(char *new_disk_name){
    // check if the file name exists within the current working directory
    int filed = open(new_disk_name,O_RDWR);
    if (filed < 0){
        std:: cerr << "unable to open disk\n";
	    return;
    }    //if found..


    Super_block *mockBlock = new Super_block;
    read(filed, mockBlock->free_block_list, 16);


	std:: map<int,int> usedBlocks;
	for (int i = 0; i < INODES;i++){
		char storage[8];

		read(filed,storage,8);
		char Bused = storage[5];
        char size = Bused & mask;
        int intsizeversion = (int) size;
		uint8_t start;
        char * newstorage = storage + 6;
        memcpy(&start, newstorage,sizeof(uint8_t));
		//char parent = temp[7];
		if (isBitISet(Bused,7)){
            for (int j = 0; j < intsizeversion; j++){
                uint8_t bytecheckmod = start + j;
                uint8_t bitcheckmod = start + j;
                int check_byte = (bytecheckmod)/8;
                int check_bit = (bitcheckmod)%8;
                check_bit = 7-check_bit;
                if (check_byte > 16){
                    continue;
                }
                if (!isBitISet(mockBlock->free_block_list[check_byte],check_bit)){
                    std:: cerr << "Error: file system in " << new_disk_name << " is inconsistent with error code 1a\n";
                    return;
                }
                if (usedBlocks.find(bytecheckmod)==usedBlocks.end()){
                    usedBlocks[start+j] = i;
                }else{
                    //error message.
                    std:: cerr << "Error: file system in " << new_disk_name << " is inconsistent with error code 1b\n";
                    return;
                }
            }

		} 
        
	}
    for (int i = 0; i < 16; i++){
        for (int j = 0; j < 8; j++){
            if (i == 0 && j == 0){
                continue;
            }
            if (isBitISet(mockBlock->free_block_list[i],7-j)){
                int arithmetic = 8*i+j;
                if (usedBlocks.find(arithmetic) == usedBlocks.end()){
                    std:: cout << "Error: file system in " << new_disk_name << " is inconsistent with error code 1c\n";
                    return;
                }
            }
        }
    }      

    int lseekval = -1*lseek(filed,0,SEEK_CUR);
    lseek(filed,lseekval,SEEK_CUR);
    lseek(filed,16,SEEK_CUR);

	std:: multimap<std:: string,uint8_t> usedBlocks_2;
	for (int i = 0; i < INODES;i++){
		char storage[8];
		read(filed,storage,8);
        char name[6];
        memcpy(name,storage,5);
        name[6] = '\0';
		char Sused = storage[5];
        char size = Sused & mask;
		uint8_t start;
        char * newstorage6 = storage +6;
        char * newstorage7 = storage + 7;
        memcpy(&start,newstorage6,sizeof(uint8_t));
		uint8_t pa;
        memcpy(&pa,newstorage7,sizeof(uint8_t));
        int sev = 7;
		if (isBitISet(Sused,sev)){
            auto range = usedBlocks_2.equal_range((std::string) name);
            if (isBitISet(Sused,7)){
                std:: string nameString = (std:: string) name;
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

    // check 3
    lseekval = -1*lseek(filed,0,SEEK_CUR);
    lseek(filed,lseekval,SEEK_CUR);
    lseek(filed,16,SEEK_CUR);

	std:: multimap<std:: string,uint8_t> usedBlocks_3;
	for (int i = 0; i < INODES;i++){
		char storage[8];
		read(filed,storage,8);
        char name[6];
        memcpy(name,storage,5);
        name[6] = '\0';
		char used = storage[5];
        char * newstorage6 = storage+6;
        char * newstorage7 = storage+7;
		uint8_t start;
        memcpy(&start,newstorage6,sizeof(uint8_t));
		uint8_t parent;
        memcpy(&parent,newstorage7,sizeof(uint8_t));
        int sev = 7;
        if (isBitISet(used,sev)){
            if ((std:: string) name == ""){
                std:: cerr << "Error: file system in " << new_disk_name << "is inconsistend with error code 3\n";
                return;
            }

        }

        else{
            for (int i = 0; i < 8; i++){
                for (int j = 0; j < 8; j++){
                    if (isBitISet(storage[i],j)){
                        std:: cerr << "Error: file system in " << new_disk_name << " is inconsistent with Error 3\n";
                        return;
                    }
                }
            }
        }
    }
    // check 4
    lseekval = -1*lseek(filed,0,SEEK_CUR);
    lseek(filed,lseekval,SEEK_CUR);
    lseek(filed,16,SEEK_CUR);

	std:: multimap<std:: string,uint8_t> usedBlocks_4;
	for (int i = 0; i < INODES;i++){
		char storage[8];

		read(filed,storage,8);
        char name[6];
        memcpy(name,storage,5);
        name[6] = '\0';
		char used = storage[5];
        char * newstorage6 = storage+6;
        char * newstorage7 = storage+7;
		uint8_t start;
        uint8_t parent;

        memcpy(&start,newstorage6,sizeof(uint8_t));
        memcpy(&parent,newstorage7,sizeof(uint8_t));

        int sev = 7;
        if (isBitISet(used,sev)){
            if (!(isBitISet(parent,sev))){
                int sInt = (unsigned int) start;
                if (sInt <1 || sInt > 127){
                    std:: cerr << "Error, File System in " << new_disk_name << " is inconsistent with Error 4\n";
                    return;
                }
            }
        }
    }

    // check 5
    int lseeksecond = -1*lseek(filed,0,SEEK_CUR);
    lseek(filed,lseeksecond,SEEK_CUR);
    lseek(filed,16,SEEK_CUR);

	std:: multimap<std:: string,uint8_t> usedBlocks_5;
	for (int i = 0; i < INODES;i++){
		//int ind = 8*i+(7-j);
		char storage[8];

		read(filed,storage,8);
        char name[6];
        memcpy(name,storage,5);
        name[6] = '\0';
		char used = storage[5];
        char size = used & mask;
        int sizeInt = (int) size;

        char * newstorage6 = storage + 6;
        char * newstorage7 = storage + 7;
		uint8_t start;
        uint8_t par;
        memcpy(&start,newstorage6,sizeof(uint8_t));

        memcpy(&par,newstorage7,sizeof(uint8_t));
        int sev = 7;
        if (isBitISet(used,sev)){
            if (isBitISet(par,sev)){
                if ((unsigned) sizeInt != 0 || (unsigned)start != 0){
                    std:: cerr<< "Error: File system in " << new_disk_name << " is inconsistent with Error 5\n";
                    return;
                }
            }
        }
    }

    int lseekthird = -1*lseek(filed,0,SEEK_CUR);
    // check 6
    lseek(filed,lseekthird,SEEK_CUR);
    lseek(filed,16,SEEK_CUR);

	std:: set<int> usedBlocks_6;
	for (int i = 0; i < INODES;i++){
		//int ind = 8*i+(7-j);
		char storage[8];

		read(filed,storage,8);
        char name[6];
        memcpy(name,storage,5);
        name[6] = '\0';
		char used = storage[5];
        char size = used & mask;
        int sizeInt = (int) size;

        char * newstorage6 = storage + 6;
        char * newstorage7 = storage + 7;

		uint8_t start;
        memcpy(&start,newstorage6,sizeof(uint8_t));
		uint8_t par;
        memcpy(&par,newstorage7,sizeof(uint8_t));

        par = par & mask;
        int sev = 7;
        if (isBitISet(used,sev)){
            if (par == MAX126){
                std:: cerr << "Error: File System in " << new_disk_name << " is inconsistent with Error 6\n";
                return;
            }
            if (par != MAX127){
                usedBlocks_6.insert(par);
            }
        }


        for (int it : usedBlocks_6){

            int lseekfinal = -1*lseek(filed,0,SEEK_CUR);
            lseek(filed,lseekfinal,SEEK_CUR);
            lseek(filed,16,SEEK_CUR);
            int arithmetic = 8*it;
            lseek(filed,arithmetic,SEEK_CUR);
            char storage[8];
            read(filed,storage,8);
            char used = storage[5];
            uint8_t par;

            char * newstorage7 = storage + 7;

            memcpy(&par,newstorage7,sizeof(uint8_t));
            int sev = 7;
            if (!isBitISet(used,sev)||!isBitISet(par,sev)){
                std:: cerr<< "Error: File System in " << new_disk_name << " is inconsistent with error 6\n";
                return;

            }
        }

    }

    int lseekfifth = -1*lseek(filed,0,SEEK_CUR);
    
    lseek(filed,lseekfifth,SEEK_CUR);
    lseek(filed,16,SEEK_CUR);

    read(filed,Disk->free_block_list,16);
    for (int i = 0; i < INODES; i++){
        Inode newI;
        char storage[8];
        read(filed,storage,8);
        memcpy(newI.name, storage,5);
        char * newstorage5 = storage + 5;
        memcpy(&(newI.used_size),newstorage5,1);
        //mask used with 0x7f
        newI.used_size &= mask;
        char * newstorage6 = storage + 6;
        char * newstorage7 = storage + 7;
        memcpy(&(newI.start_block),newstorage6,1);
        memcpy(&(newI.dir_parent),newstorage7,1);
        // mask parent with 0x7f
        newI.dir_parent &= mask;

        Disk->inode[i] = newI;
    }
    Directorylocation =(uint8_t) MAX127;
    Dname = (std:: string) new_disk_name;

}

void fs_create(char name[5], int size){
    if (Dname.empty()){
        std::cout << "No disk is mounted\n";
        return;
    }
    // search for the name that matches with the current name.

    // delete the file or directory with char name[5]

}

void fs_read(char name[5], int block_num){

}


void fs_cd(char name[5]){
    // changes the given directory to the specified directory
    
    if (strcmp(name, ".") != -1){
        return;
    }
    if (strcmp(name,"..")){
        if (Directorylocation != MAX127){
            Directorylocation = Disk->inode[Directorylocation].dir_parent & mask;
        } 
        return;
    }
    for (int i = 0; i < INODES;i++){
        uint8_t parent = Disk->inode[i].dir_parent;
        parent = parent & 0x7f;
        if (strcmp(Disk->inode[i].name,name) == 0 && (parent == Directorylocation)){
            if (Disk->inode[i].dir_parent & 0x80){
                Directorylocation = i;
                return;
            }
        }
    }
    std:: cerr << "Error, File/directory" << name << "does not exist\n";
}

void fs_write(char name[5], int block_num){

}

void fs_buff(uint8_t buff[1024]){
    memset(buffer,buff,sizeof(buffer));
    memcpy(buffer,buff,sizeof(buffer));
}

void fs_defrag(void){

}

void fs_resize(char name[5], int new_size){

}

void fs_ls(void){

}

std::vector<std::string> tokenize(const std::string &str, const char *delim) {
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


int parseCommand(std:: string command){
    std:: vector<std:: string> tokenizer = tokenize(command.c_str()," ");

    if (tokenizer[0] == "M" && tokenizer.size() == 2){
        char *arr = new char[tokenizer[1].size()+1];
        strcpy(arr, tokenizer[1].c_str());
        fs_mount(arr);
        delete[] arr;
        return 0;
    }
    else if (tokenizer[0] == "C" && tokenizer.size() == 3){
        if (tokenizer[1].size() > 5){
            return -1;
        }
        char *arr = new char[tokenizer[1].size()+1];
        strcpy(arr,tokenizer[1].c_str());
        fs_create(arr,stoi(tokenizer[2]));
        delete[] arr;
        return 0;
    }
    else if (tokenizer[0] == "D" && tokenizer.size() == 2){

    }
    else if (tokenizer[0] == "R" && tokenizer.size() == 3){

    }
    else if (tokenizer[0] == "W" && tokenizer.size() == 3){

    }
    else if (tokenizer[0] == "B" && tokenizer.size() == 2){

    }
    else if (tokenizer[0] == "L" && tokenizer.size() == 1){

    }
    else if (tokenizer[0] == "E" && tokenizer.size() == 3){

    }
    else if (tokenizer[0] == "O" && tokenizer.size() == 1){

    }
    else if (tokenizer[0] == "Y" && tokenizer.size() == 2){
        char *arr = new char[tokenizer[1].size()+1];
        strcpy(arr,tokenizer[1].c_str());
        fs_cd(arr);
        delete[] arr;
        return 0;
    }

}

int main(int argc, char *argv[]){
    if (argc != 2){
        std:: cerr << "Usage: ./fs <input_file>\n";
    }

    Disk = new Super_block;
    mockBlock = new Super_block;

    memset(buffer,0,sizeof(buffer));

    char * input = argv[1];
    std:: string line;
    std:: ifstream commands (input);
    if (commands.is_open()){
        int lnumber = 1;
        while (getline(commands,line)){
            if (parseCommand(line)==-1){
                std:: cerr << "Command Error: " << input << ", " << lnumber << "\n";
            }
            lnumber = lnumber + 1;
        }
    }

    delete Disk;
    delete mockBlock;
}



