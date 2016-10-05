#include <stdio.h>
#include <stdlib.h>
#include "val.c"
#include <string.h>
#include <stdint.h>

//File allocation table. int is next value
typedef int FAT[1000];
struct DIR_ENT {
    int start;
    int size;
    int ifdir;
    char name[255];
};
typedef struct DIR_ENT DIR[1000];
typedef char DATA[1000];
/*
 * Read FAT
 * returns pointer to FAT
 * */
int read_fat(FAT *fat)
{
    FILE *file = fopen(PATH_FAT, "r");
    if (!file) {
        perror("Could not open FAT");
        return 1;
    }
    //find the file size
    long sf;
    fseek(file, 0L, SEEK_END);
    sf = ftell(file);
    rewind(file);
    //Read whole file
    if (fread(fat, (size_t)sf, 1, file) != 1) {
        perror("Could not read FAT");
        return  1;
    }
    fclose(file);
    return 0;
}

/*
 * Returns 1 if error
 * */
int write_fat(FAT *point)
{
    FILE *file = fopen(PATH_FAT, "w");
    if (!file) {
        perror("Could not open FAT");
        return 1;
    }
    if (fwrite(point, sizeof(point), 1, file) != 1) {
        perror("Could not write FAT");
        return 1;
    }
    fclose(file);
    return 0;
}

//Write value to directory table
int write_dir(DIR *dir)
{
    FILE *file = fopen(PATH_DIR, "w");
    if (!file) {
        perror("Could not open DIR");
        return 1;
    }
    if (fwrite(dir, sizeof(dir), 1, file) != 1) {
        perror("Could not write DIR");
        return 1;
    }
    fclose(file);
    return 0;
}

int read_dir(DIR *dir)
{
    FILE *file = fopen(PATH_DIR, "r");
    if (!file) {
        perror("Could not open DIR");
        return 1;
    }
    //find the file size
    long sf;
    fseek(file, 0L, SEEK_END);
    sf = ftell(file);
    rewind(file);
    //Read whole file
    if (fread(dir, (size_t)sf, 1, file) != 1) {
        perror("Could not read DIR");
        return  1;
    }
    fclose(file);
    return 0;
}

int write_data(DATA *data)
{
    FILE *file = fopen(PATH_DATA, "w");
    if (!file) {
        perror("Could not open DATA");
        return 1;
    }
    if (fwrite(data, sizeof(DATA), 1, file) != 1) {
        perror("Could not write DATA");
        return 1;
    }
    fclose(file);
    return 0;
}
/*
* Write single byte from data
*/
int wbyte_data()
{

}

int read_data(DATA *data)
{
    FILE *file = fopen(PATH_DATA, "r");
    if (!file) {
        perror("Could not open DATA");
        return 1;
    }
    //find the file size
    long sfile;
    fseek(file, 0L, SEEK_END);
    sfile = ftell(file);
    rewind(file);
    //Read whole file
    if (fread(data, (size_t)sfile, 1, file) != 1) {
        perror("Could not read DATA");
        return  1;
    }
    fclose(file);
    return 0;
}

/*
* Read single byte from data.
* int - byte position.
* char - char to read to.
*/
int rbyte_data(int pos, char *dest)
{
    FILE *file = fopen(PATH_DATA, "r+");
    if (!file) {
        perror("Could not open DATA");
        return 1;
    }
    fseek(file, pos, SEEK_SET);
    if (fread(dest, 1, 1, file) != 1) {
        perror("Could not read DATA");
        return  1;
    }
    fclose(file);
    return 0;
}

int test_read()
{
    FAT fat;
    if (read_fat(&fat)) {
        return 1;
    }
    DIR dir;
    if (read_dir(&dir)) {
        return 1;
    }
    DATA data;
    char ch;
    if (rbyte_data(777, &ch)) {
        return 1;
    }
    printf("FAT: %s ", dir[0].name);
    printf("DIR: %d ", fat[0]);
    printf("DATA: %c\n", ch);
    return 0;
}

int init()
{
    FAT fat;
    fat[0] = 1;
    if (write_fat(&fat)) {
        return 1;
    }
    DIR dir;
    strcpy(dir[0].name, "unknown");
    if (write_dir(&dir)) {
        return 1;
    }
    DATA data;
    memset(data, '_', sizeof(DATA));
    data[777] = 'R';
    // for (int i = 0; i < 1000; ++i)
    // {
    //     strcpy(&data[i], "h");
    //     // printf("%d\n", i);
    // }
    if (write_data(&data)) {
        return 1;
    }
    return 0;
}