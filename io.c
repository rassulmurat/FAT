#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "values.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "tree.c"

int getFreeFat(int fd, int *pos);
int findMD(int fd, const char *path, struct dir_ent *md, int *mdpos);
int getFreeMD(int fd, struct dir_ent *md, int *pos);
int initvals();
int initpopul();
int wtfat(int fd, int block, int pos);
int wrdir(int fd, struct dir_ent *DIR, int pos);
int wtdblock(int fd, char *block, int pos);
int initpoptree();
int rddir(int fd, struct dir_ent *DIR, int pos);

int init()
{
    initvals();
    struct stat buf;
    //Check if file exists
    int err = stat(file_path, &buf);
    if (err != 0) {
        initpopul();
    }
    initpoptree();
    return 0;
}

int initpoptree()
{
    int fd = open(file_path, O_RDONLY);
    struct dir_ent *md = malloc(sizeof(struct dir_ent));
    for (int i = 0; i < dirnum; ++i)
    {
        rddir(fd, md, i);
        if (strcmp(md->path, "") != 0) {
            insert(md->path);
        }
    }
    close(fd);
    puts("Tree Initialized");
    return 0;
}

int initpopul()
{
    int fd = open(file_path, O_WRONLY | O_RDONLY | O_CREAT, 0777);
    //Populate dirs
    struct dir_ent md = {.isdir = FREE_BLOCK};
    for (int i = 0; i < dirnum; ++i) {
        wrdir(fd, &md, i);
    }
    md.isdir = 1;
    strcpy(md.path, "/");
    wrdir(fd, &md,0);
    puts("Created dir");
    //Populate fats
    for (int j = 0; j < blnum; ++j) {
        int block = FREE_BLOCK;
        wtfat(fd, block, j);
    }
    puts("Created fat");
    //Populate data
    char *block = malloc(BLOCK_SIZE*sizeof(char));
    memset(block, '\0', BLOCK_SIZE*sizeof(char));
    for (int i = 0; i < blnum; ++i) {
        wtdblock(fd, block, i);
    }
    puts("Created data");
    close(fd);
    return 0;
}

/*
 * Initializes values for alter use.
 * */
int initvals()
{
    blnum    = totalSize/BLOCK_SIZE;
    sizedir  = sizeof(struct dir_ent);
    fat_pos  = dirnum*sizedir;
    sizefat  = blnum*sizeof(int);
    data_pos = fat_pos + blnum*sizeof(int);
    sizedata = blnum*BLOCK_SIZE*sizeof(char);
}

/*
 *  Read dir at position.
 *  fd - file desc.
 * */
int rddir(int fd, struct dir_ent *DIR, int pos)
{
//    DIR = malloc(sizedir);
    lseek(fd, pos*sizedir, SEEK_SET);
    int ret = read(fd, DIR, sizedir);
    if (ret != sizedir) {
        perror("Could not read DIR");
        return 1;
    }
    return 0;
}

int rdfat(int fd, int *fat, int pos)
{
    lseek(fd, fat_pos + pos*sizeof(int), SEEK_SET);
    int err = read(fd, fat, sizeof(int));
    if (err != sizeof(int)) {
        puts("Did not read full fat");
        return 1;
    }
    return 0;
}

/*
 *  Read block of data
*/
int rddblock(int fd, char *block, int pos)
{
    lseek(fd, data_pos + pos*BLOCK_SIZE, SEEK_SET);
    int err = read(fd, block, BLOCK_SIZE*sizeof(char));
    printf("Block is = %s\n", block);
    if (err != BLOCK_SIZE*sizeof(char)) {
        perror("Could not read data block");
        return 1;
    }
    return 0;
}

int wtdblock(int fd, char *block, int pos)
{
    lseek(fd, pos*BLOCK_SIZE + data_pos, SEEK_SET);
    int err = write(fd, block, BLOCK_SIZE);
    printf("Block is = %s\n", block);
    if (err != BLOCK_SIZE*sizeof(char)) {
        perror("Didnt write full data block");
        return 1;
    }
    return 0;
}

int wtfat(int fd, int block, int pos)
{
    lseek(fd, fat_pos + sizeof(int)*pos, SEEK_SET);
    int err = write(fd, &block, sizeof(int));
    if (err != sizeof(int)) {
        perror("Didnt write fill fat");
        return 1;
    }
    return 0;
}

int wrdir(int fd, struct dir_ent *DIR, int pos)
{
    lseek(fd, pos*sizedir, SEEK_SET);
    int ret = write(fd, DIR, sizedir);
    if (ret != sizedir) {
        perror("Could not write DIR");
        return 1;
    }
    return 0;
}

int mkDir(const char *path)
{
    int fd = open(file_path, O_RDWR);
    if (fd < 0) {
        perror("Bad fd");
        return 1;
    }
    //Allocate MD
    struct dir_ent *md = malloc(sizeof(struct dir_ent));
    int pos;
    int err = getFreeMD(fd, md, &pos);
    if (err) {
        return 1;
    }
    md->isdir = 1;
    strcpy(md->path, path);
    err = wrdir(fd, md, pos);
    if (err) {
        return 1;
    }
    insert(md->path);
    return 0;
}

/*
 * Read file.
 * */
int fRead(const char *path, char *str)
{
    int fd = open(file_path, O_RDONLY);
    //Find MD
    struct dir_ent *md = malloc(sizeof(struct dir_ent));
    int mdpos;
    int err = findMD(fd, path, md, &mdpos);
    if (err) {
        return 1;
    }
    //Get curr and check if dir
    if (md->isdir == 1) {
        puts("Path is dir");
        return 1;
    }
    int curr = md->start;
    int next;
    char *block = malloc(BLOCK_SIZE*sizeof(char));
    //Start reading
    for (int i = 0; i < md->size; ++i) {
        err = rddblock(fd, block, curr);
        if (err) {
            return 1;
        }
        printf("Block has: %s\n", block);
        for (int j = 0; j < BLOCK_SIZE; ++j) {
            str[i*BLOCK_SIZE + j] = block[j];
        }
        err = rdfat(fd, &next, curr);
        if (err) {
            return 1;
        }
        curr = next;
    }
}

/*
 * Write file
 * */
int fWrite(const char *file, const char *path)
{
    int fd = open(file_path, O_RDWR);
    if(fd < 0) {
        perror("BAD FD");
        return 1;
    }
    //Get free metadata
    struct dir_ent *md;
    int mdpos = 0;
    md = malloc(sizedir);
    int err = findMD(fd, path, md, &mdpos);
    if (err) {
        err = getFreeMD(fd, md, &mdpos);
        if (err) {
            return 1;
        }
    }
    //Populate md
    md->isdir = 0;
    strcpy(md->path, path);
    md->size = (strlen(file) * sizeof(char)) / BLOCK_SIZE;
    if (md->size == 0) {
        md->size = 1;
    }
    printf("%d\n", md->size);
    //Start writing
    int curr;
    err = getFreeFat(fd, &curr);
    printf("Writing to md: %d\n", mdpos);
    if (err) {
        return 1;
    }
    md->start = curr;
    int next = END_BLOCK;
    char *block = malloc(BLOCK_SIZE*sizeof(char));
    memset(block, '\0', BLOCK_SIZE);
    for (int i = 0; i < md->size; ++i) {
        for (int j = 0; j < BLOCK_SIZE; ++j) {
            if (strlen(file) > i*BLOCK_SIZE + j) {
                block[j] = file[i*BLOCK_SIZE + j];
            }
        }
        err = wtdblock(fd, block, curr);
        printf("block: %s\n", block);
        printf("fat pos  wr %d\n", curr);
        if (err) {
            return 1;
        }
        wtfat(fd, next, curr);
        if (i + 1 != md->size) {
            err = getFreeFat(fd, &next);
            wtfat(fd, next, curr);
            curr = next;
        }
        if (err) {
            return 1;
        }
    }
    wrdir(fd, md, mdpos);
    char *npath = malloc(strlen(path)*sizeof(char));
    strcpy(npath, path);
    insert(npath);
    close(fd);
    return 0;
}

/*
* Check if exists in file system
*/
int exists(const char *path)
{
    int fd = open(file_path, O_RDONLY);
    if(fd < 0) {
        perror("Bad fd");
        return 1;
    }
    struct dir_ent *md = malloc(sizeof(struct dir_ent));
    int mdpos;
    int err = findMD(fd, path, md, 0);
    if (err) {
        return 1;
    }
    return 0;
}

/*
*	Get next free block
*/
int getFreeFat(int fd, int *pos)
{
    int fat;
    for (int i = 0; i < blnum; ++i) {
        rdfat(fd, &fat, i);
        if (fat == FREE_BLOCK) {
            *pos = i;
            return 0;
        }
    }
    puts("Could not find free blocks");
    return 1;
}

int getFreeMD(int fd, struct dir_ent *md, int *pos)
{
    for (int i = 0; i < dirnum; ++i) {
        int err = rddir(fd, md, i);
        if (err) {
            return 1;
        }
        if(md->isdir == FREE_BLOCK) {
            *pos = i;
            return 0;
        }
    }
    puts("Could not find free MD");
    return 1;
}

int findMD(int fd, const char *path, struct dir_ent *md, int *mdpos)
{
    for (int i = 0; i < dirnum; ++i) {
        rddir(fd, md, i);
        if(strcmp(md->path, path) == 0) {
            *mdpos = i;
            printf("found md at pos %d, md->start %d\n", i, md->start);
            
            return 0;
        }
    }
    puts("Could not find MD");
    return 1;
}