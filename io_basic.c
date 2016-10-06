#include <stdio.h>
#include <stdlib.h>
#include "val.c"
#include <string.h>
#include <stdint.h>

/*
 * Read FAT
 * returns fater to FAT
 * */
int read_fat(FAT *fat)
{
    FILE *file = fopen(PATH_FAT, "r");
    if (!file) {
        perror("Could not open FAT");
        return 1;
    }
    //Read whole file
    if (fread(fat, sizeof(FAT), 1, file) != 1) {
        perror("Could not read FAT");
        return  1;
    }
    fclose(file);
    return 0;
}

/*
 * Returns 1 if error
 * */
int write_fat(FAT *fat)
{
    FILE *file = fopen(PATH_FAT, "w");
    if (!file) {
        perror("Could not open FAT");
        return 1;
    }
    if (fwrite(fat, sizeof(FAT), 1, file) != 1) {
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
    if (fwrite(dir, sizeof(DIR), 1, file) != 1) {
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
    //Read whole file
    if (fread(dir, sizeof(DIR), 1, file) != 1) {
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
*   Read full data file.
*/
int read_data(DATA *data)
{
    FILE *file = fopen(PATH_DATA, "r");
    if (!file) {
        perror("Could not open DATA");
        return 1;
    }
    //Read whole file
    if (fread(data, sizeof(DATA), 1, file) != 1) {
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
    pos = pos + 1;
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

/*
*   Write single byte to data.
*   int - byte Position
*   char - Byte.
*/
int wbyte_data(int pos, char ch)
{
    pos = pos + 1;
    FILE *file = fopen(PATH_DATA, "r+");
    if (!file) {
        perror("Could not open DATA");
        return 1;
    }
    fseek(file, pos, SEEK_SET);
    if (fwrite(&ch, 1, 1, file) != 1) {
        perror("Could not write DATA");
        return 1;
    }
    fclose(file);
    return 0;
}
