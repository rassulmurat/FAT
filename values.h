struct dir_ent {
    char 	path[1000];
    int 	start;
    //Size is in blocks
    int 	size;
    int		isdir;
};

const int FREE_BLOCK = -1;
const int END_BLOCK  = -2;
const int BLOCK_SIZE = 32;

// const int totalSize = 1073741824;
const int totalSize = 1000;


const int dirnum = 100;
/*
 * Number of blocks
 * Will be initialized in initvals()
 */
int blnum    = 0;
int fat_pos  = 0;
int data_pos = 0;
int sizedata = 0;
int sizefat  = 0;
int sizedir  = 0;

char *file_path = "root.bin";