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
int mask = 0x7f;
int mask0x80 = 0x80;



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
    read(filed, mockBlock->free_block_list, 2*size_macro);


	std:: map<int,int> map1;
	for (int i = 0; i < INODES;i++){
		char storage[size_macro];

		read(filed,storage,size_macro);
		char Bused = storage[USED_BLOCK];
        uint8_t size = Bused & 0x7F;
        int intsizeversion = unsigned(size);
		uint8_t start;

        memcpy(&start, storage+6,sizeof(uint8_t));
		//char parent = temp[7];
		if (isBitISet(Bused,7)){
            for (int j = 0; j < intsizeversion; j++){
                uint8_t bytecheckmod = start + j;
                uint8_t bitcheckmod = start + j;
                int check_byte = (bytecheckmod)/size_macro;
                int check_bit = (bitcheckmod)%size_macro;
                check_bit = 7-check_bit;
                if (check_byte > 2*size_macro){
                    continue;
                }
                if (!isBitISet(mockBlock->free_block_list[check_byte],check_bit)){
                    std:: cerr << "Error: file system in " << new_disk_name << " is inconsistent with error code 1\n";
                    return;
                }
                if (map1.find(bytecheckmod)==map1.end()){
                    map1[start+j] = i;
                }else{
                    //error message.
                    std:: cerr << "Error: file system in " << new_disk_name << " is inconsistent with error code 1b\n";
                    return;
                }
            }

		} 
        
	}
    for (int i = 0; i < 2*size_macro; i++){
        for (int j = 0; j < size_macro; j++){
            if (i == 0 && j == 0){
                continue;
            }
            if (isBitISet(mockBlock->free_block_list[i],7-j)){
                int arithmetic = size_macro*i+j;
                if (map1.find(arithmetic) == map1.end()){
                    std::cout<<arithmetic<<"\n";
                    std:: cout << "Error: file system in " << new_disk_name << " is inconsistent with error code 1c\n";
                    return;
                }
            }
        }
    }      

    int lseekval = -1*lseek(filed,0,SEEK_CUR);
    lseek(filed,lseekval,SEEK_CUR);
    lseek(filed,2*size_macro,SEEK_CUR);

	std:: multimap<std:: string,uint8_t> usedBlocks_2;
	for (int i = 0; i < INODES;i++){
		char storage[size_macro];
		read(filed,storage,size_macro);
        char name[NAME_SIZE_LIMIT];
        memcpy(name,storage,USED_BLOCK);
        name[NAME_SIZE_LIMIT] = nullchar;
		char Sused = storage[USED_BLOCK];
        //char size = Sused & mask;
		uint8_t start;
        
        memcpy(&start,storage+6,sizeof(uint8_t));
		uint8_t pa;
        memcpy(&pa,storage + 7,sizeof(uint8_t));
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
    lseek(filed,2*size_macro,SEEK_CUR);

	std:: multimap<std:: string,uint8_t> usedBlocks_3;
	for (int i = 0; i < INODES;i++){
		char storage[size_macro];
		read(filed,storage,size_macro);
        char name[NAME_SIZE_LIMIT];
        memcpy(name,storage,USED_BLOCK);
        name[NAME_SIZE_LIMIT] = nullchar;
		char used = storage[USED_BLOCK];
		uint8_t start;
        memcpy(&start,storage+6,sizeof(uint8_t));
		uint8_t parent;
        memcpy(&parent,storage+7,sizeof(uint8_t));
        int sev = 7;
        if (isBitISet(used,sev)){
            if ((std:: string) name == ""){
                std:: cerr << "Error: file system in " << new_disk_name << "is inconsistend with error code 3\n";
                return;
            }

        }

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
    // check 4
    lseekval = -1*lseek(filed,0,SEEK_CUR);
    lseek(filed,lseekval,SEEK_CUR);
    lseek(filed,2*size_macro,SEEK_CUR);

	std:: multimap<std:: string,uint8_t> usedBlocks_4;
	for (int i = 0; i < INODES;i++){
		char storage[size_macro];

		read(filed,storage,size_macro);
        char name[NAME_SIZE_LIMIT];
        memcpy(name,storage,USED_BLOCK);
        name[NAME_SIZE_LIMIT] = nullchar;
		char used = storage[USED_BLOCK];
		uint8_t start;
        uint8_t parent;

        memcpy(&start,storage+6,sizeof(uint8_t));
        memcpy(&parent,storage+7,sizeof(uint8_t));

        int sev = 7;
        if (isBitISet(used,sev)){
            if (!(isBitISet(parent,sev))){
                int sInt = (unsigned int) start;
                if (sInt > MAX127 ||sInt <MIN1){
                    std:: cerr << "Error, File System in " << new_disk_name << " is inconsistent with Error 4\n";
                    return;
                }
            }
        }
    }

    // check 5
    int lseeksecond = -1*lseek(filed,0,SEEK_CUR);
    lseek(filed,lseeksecond,SEEK_CUR);
    lseek(filed,2*size_macro,SEEK_CUR);

	std:: multimap<std:: string,uint8_t> usedBlocks_5;
	for (int i = 0; i < INODES;i++){
		char storage[size_macro];

		read(filed,storage,size_macro);
        char name[NAME_SIZE_LIMIT];
        memcpy(name,storage,USED_BLOCK);
        name[NAME_SIZE_LIMIT] = nullchar;
		char used = storage[USED_BLOCK];
        char size = used & mask;
        int sizeInt = (int) size;

		uint8_t start;
        uint8_t par;
        memcpy(&start,storage+6,sizeof(uint8_t));

        memcpy(&par,storage+7,sizeof(uint8_t));
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
    lseek(filed,2*size_macro,SEEK_CUR);

	std:: set<int> usedBlocks_6;
	for (int i = 0; i < INODES;i++){
		char storage[size_macro];

		read(filed,storage,size_macro);
        char name[NAME_SIZE_LIMIT];
        memcpy(name,storage,USED_BLOCK);
        name[NAME_SIZE_LIMIT] = nullchar;
		char used = storage[USED_BLOCK];
        //char size = used & mask;
        //int sizeInt = (int) size;

		uint8_t start;
        memcpy(&start,storage+6,sizeof(uint8_t));
		uint8_t par;
        memcpy(&par,storage+7,sizeof(uint8_t));

        par = par & 0x7F;
        if (isBitISet(used,7)){
            if (par == MAX126){
                std:: cerr << "Error: File System in " << new_disk_name << " is inconsistent with Error 6\n";
                return;
            }
            if (par != MAX127){
                std::cout<<"node "<<i<<" has parent "<<unsigned(par)<<"\n";
                usedBlocks_6.insert(par);
            }
        }


        for (int it : usedBlocks_6){

            int lseekfinal = -1*lseek(filed,0,SEEK_CUR);
            lseek(filed,lseekfinal,SEEK_CUR);
            lseek(filed,2*size_macro,SEEK_CUR);
            int arithmetic = size_macro*it;
            lseek(filed,arithmetic,SEEK_CUR);
            char storage[size_macro];
            read(filed,storage,size_macro);
            char used = storage[USED_BLOCK];
            uint8_t par;

            memcpy(&par,storage+7,sizeof(uint8_t));
            int sev = 7;
            if (!isBitISet(used,sev)||!isBitISet(par,sev)){
                std::cout<<"node "<<it<<"\n";
                std::cout<<"parent "<<unsigned(par)<<"\n";
                std:: cerr<< "Error: File System in " << new_disk_name << " is inconsistent with error 6a\n";
                return;
            }
        }

    }

    lseek(filed,0,SEEK_SET);
    read(filed,Disk->free_block_list,16);
    for (int i = 0; i < INODES; i++){
        Inode newI;
        char storage[size_macro];
        read(filed,storage,size_macro);
        memcpy(newI.name, storage,USED_BLOCK);
        char * newstorage5 = storage + USED_BLOCK;
        memcpy(&(newI.used_size),newstorage5,1);
        //mask used with 0x7f
        newI.used_size &= mask;
        memcpy(&(newI.start_block),storage+6,1);
        memcpy(&(newI.dir_parent),storage+7,1);
        // mask parent with 0x7f
        newI.dir_parent &= mask;

        Disk->inode[i] = newI;
    }
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

    while (i < INODES){
        if (strcmp(Disk->inode[i].name,"") == 0){
            //inode that is not in use is found 
            break;
        }
        i = i + 1;
    }
    if (i == INODES){
        std:: cout <<  "Error: Superblock in disk "<< Dname << " is full, cannot create " << name << "\n";
        return;
    }

    for (int j = 0; j < INODES; j++){
        if (strncmp(Disk->inode[j].name,name,USED_BLOCK) == 0){
            uint8_t parent = Disk->inode[i].dir_parent & 0x7f;
            if(parent == Directorylocation){
                std:: cout << "Error: File or directory " << name <<" already exists\n";
                return;
            } 
        }
    }
    if (strcmp(name,dot.c_str()) == 0){
        std:: cout <<"Error: File or directory " << name <<" already exists\n";
        return;
    }

    if (strcmp(name,doubledot.c_str()) == 0){
        std:: cout <<"Error: File or directory " << name <<" already exists\n";
        return;
    }

    //create a new file within the current working directory
    uint8_t start;
    uint8_t dir;
    if (size == 0){
        // create a directory
        dir = 1;
        dir = dir << 7;
        start = 0;
    }else{
        dir = 0;
        int count = 0;
        int j = 0;
        int k = 7;
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
            if (count == size){
                break;
            }
        }
        
        int blocknum =8*j+7-k;
        start = blocknum - size + 1;
        //mark
        mask = 1;
        while (count > 0){
            Disk->free_block_list[j] |= (mask << k);
            count--;
            k++;
            if (k > 7){
                j--;
                k = 0;
            }
        }
    }
    Disk->inode[i].dir_parent = Directorylocation|dir;
    Disk->inode[i].used_size = size|0x80;
    Disk->inode[i].start_block = start;
    memcpy(Disk->inode[i].name, name,5);

    int file_d = open(Dname.c_str(),O_RDWR);
    if (file_d < 0){
        std:: cerr << "unable to open disk\n";
	    return;
    }

    write(file_d,Disk->free_block_list, 16);

    for (int z = 0; z < INODES; z++){
        Inode inode = Disk->inode[z];
        char *name = new char[5];
        std:: memcpy(name,inode.name,5);
        int num = write(file_d,name,5);
        lseek(file_d,5-num,SEEK_CUR);
        write(file_d,&inode.used_size,1);
        write(file_d,&inode.start_block,1);
        write(file_d,&inode.dir_parent,1);
        delete[] name;

    }
    close(file_d);

}

void fs_delete(char name[5]){
    if (Dname.empty()){
        std::cout << "No disk is mounted\n";
        return;
    }
    for (int j = 0; j < INODES; j++){
        if (strncmp(Disk->inode[j].name,name,USED_BLOCK) == 0){
            uint8_t parent = Disk->inode[j].dir_parent & 0x7f;
            if(parent == Directorylocation){
                // i is the inode that we want
                // check if inode[i] is a directory or file
                if (Disk->inode[j].dir_parent&0x80){
                    // is a directory
                    std:: map<int, Inode> Dchildren;
                    for (int k = 0; k < INODES; k++){
                        //find all inodes with the parent j
                        if ((Disk->inode[k].dir_parent & 0x7f) == parent){
                            Dchildren.insert(std::pair<int,Inode>(k,Disk->inode[k]));
                        }
                    }

                    for (auto itr = Dchildren.begin(); itr != Dchildren.end(); itr++){
                        fs_delete(it->second.name);
                    }
                    memset(Disk->inode[j].name,0,5);
                    memset(Disk->inode[j].start_block,0,1);
                    memset(Disk->inode[j].used_size,0,1);
                    memset(Disk->inode[j].dir_parent,0,1);
                }else{

                }
            } 
        }
    }
}

void fs_read(char name[5], int block_num){
    if (Dname.empty()){
        std::cout << "No disk is mounted\n";
        return;
    }
}


void fs_cd(char name[5]){
    // changes the given directory to the specified directory
    if (strcmp(name,doubledot.c_str()) != -1){
        if (Directorylocation != MAX127){
            Directorylocation = Disk->inode[Directorylocation].dir_parent & mask;
            std::cout<< Directorylocation << "parent \n";
        } 
        return;
    }
    else if (strcmp(name, dot.c_str()) != -1){
        return;
    }

    for (uint8_t i = 0; i < INODES;i++){
        uint8_t parent = Disk->inode[i].dir_parent;
        parent = parent & mask;
        if (strcmp(Disk->inode[i].name,name) == 0 && (parent == Directorylocation)){
            if (!(Disk->inode[i].dir_parent & mask0x80)){
                continue;
            }
            Directorylocation = i;
            std::cout<< Directorylocation << "\n";
            return;
        }
    }
    std:: cerr << "Error, File/directory" << name << "does not exist\n";
}

void fs_write(char name[5], int block_num){
    if (Dname.empty()){
        std::cout << "No disk is mounted\n";
        return;
    }
}

void fs_buff(uint8_t buff[1024]){
    memset(buffer,0,sizeof(buffer));
    memcpy(buffer,buff,sizeof(buffer));
}

void fs_defrag(void){
    if (Dname.empty()){
        std::cout << "No disk is mounted\n";
        return;
    }
}

void fs_resize(char name[5], int new_size){
    if (Dname.empty()){
        std::cout << "No disk is mounted\n";
        return;
    }
}

void fs_ls(void){
    if (Dname.empty()){
        std::cout << "No disk is mounted\n";
        return;
    }
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


int readInput(std:: string command){
    std:: vector<std:: string> tokenizer = tokenize(command.c_str()," ");

    if (tokenizer[0] == "M" && tokenizer.size() == 2){

        char *arr = new char[tokenizer[1].size()+1];
        strcpy(arr, tokenizer[1].c_str());
        fs_mount(arr);
        delete[] arr;
        return 0;
    }
    else if (tokenizer[0] == "C" && tokenizer.size() == 3){
        if (tokenizer[1].size() > USED_BLOCK){
            std:: cerr << "Command Error: <input file name>, <line number>";
            return -1;
        }
        if (stoi(tokenizer[2]) > 127 || stoi(tokenizer[2]) < 0){
            return -1;
        }
        char *arr = new char[tokenizer[1].size()+1];
        strcpy(arr,tokenizer[1].c_str());
        int size = stoi(tokenizer[2]);
        fs_create(arr,size);
        delete[] arr;
        return 0;
    }
    else if (tokenizer[0] == "D" && tokenizer.size() == 2){
        
        char *arr = new char[tokenizer[1].size()+1];
        fs_delete(arr);
        delete[] arr;
        return 0;
    }
    else if (tokenizer[0] == "R" && tokenizer.size() == 3){
        char *arr = new char[tokenizer[1].size() + 1];
        strcpy(arr, tokenizer[1].c_str());
        int bnumber = stoi(tokenizer[2]);
        fs_read(arr, bnumber);
        delete[] arr;
        return 0;
    }
    else if (tokenizer[0] == "W" && tokenizer.size() == 3){
        char *arr = new char[tokenizer[1].size() + 1];
        strcpy(arr, tokenizer[1].c_str());
        int bnumber = stoi(tokenizer[2]);
        fs_read(arr, bnumber);
        delete[] arr;
        return 0;
    }
    else if (tokenizer[0] == "B" && tokenizer.size() >= 2){
        
        if ((command.size() - command.find("B")> 1026)){
            // command exceeds the size of the buffer
            std:: cerr << "command exceeds the size of the buffer.\n";
            return -1;
        }
        uint8_t *buff = new uint8_t[BUFFER_SIZE_PRESET];
        memset(buff,0,1024);
        copy(command.begin() + command.find("B"),command.end(), buff);
        fs_buff(buff);
        delete[] buff;
        return 0;
    }
    else if (tokenizer[0] == "L" && tokenizer.size() == 1){
        fs_ls();
        return 0;
    }
    else if (tokenizer[0] == "E" && tokenizer.size() == 3){
        char *arr = new char[tokenizer[1].size() + 1];
        strcpy(arr, tokenizer[1].c_str());
        int bnumber = stoi(tokenizer[2]);
        fs_resize(arr,bnumber);
        delete[] arr;
        return 0;

    }
    else if (tokenizer[0] == "O" && tokenizer.size() == 1){
        fs_defrag();
        return 0;
    }
    else if (tokenizer[0] == "Y" && tokenizer.size() == 2){
        char *arr = new char[tokenizer[1].size()+1];
        strcpy(arr,tokenizer[1].c_str());
        fs_cd(arr);
        delete[] arr;
        return 0;
    }
    // std:: cerr << "Command Error: <input file name>, <line number>\n";
    return -1;

}

int main(int argc, char *argv[]){
    if (argc != 2){
        std:: cerr << "Usage: ./fs <input_file>\n";
    }
    //clear the given buffer...
    memset(buffer,0,sizeof(buffer));
    std:: string line;
    char * input = argv[1];
    std:: ifstream commands (input);

    Disk = new Super_block;
    mockBlock = new Super_block;

    if (!commands.is_open()){
        return -1;
    }else{
        int lnumber = 1;
        while (getline(commands,line)){
            if (readInput(line)==-1){
                std:: cerr << "Command Error: " << input << ", " << lnumber << "\n";
            }
            lnumber = lnumber + 1;
        }
    }
    delete Disk;
    delete mockBlock;
    return 0;
}
