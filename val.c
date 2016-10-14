//
// Created by user on 4.10.16.
//
int const SIZE_FAT = 8;
int const PATH_SIZE = 500;

char *NAME_FAT = "/FAT";
char *NAME_DIR = "/DIR";
char *NAME_DATA = "/DATA";
char *PATH_FAT;
char *PATH_DIR;
char *PATH_DATA;
// int CELL_NUM = 1000;

int const END_BLOCK = -1;
int const FREE_BLOCK = -2;

//File allocation table. int is next value
typedef int FAT[1000];
struct DIR_ENT {
    int start;
    int size;
    //isdir 1 is directory
    int isdir;
    char path[500];
};
typedef struct DIR_ENT **DIR;
typedef char DATA[1000];