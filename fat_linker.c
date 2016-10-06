#include "io_basic.c"
#include "fat_linker.h"
// #include "fat_funcs.c"

static FAT global_fat;
static DIR global_dir;

/*
*	char path - is the path to a file.
*	int numb - is bumber of bytes to read.
*	char *dest - is pointer to string to read to.
*/
int read_file(char *path, int numb, char *dest, int pointer_pos)
{
	update();	
	struct DIR_ENT dent;
	int mdpos = md_find(path);
	if (mdpos == -1) {
		return 1;
	}
	dent = global_dir[mdpos];
	int pos = seek(pointer_pos, &dent);
	for (int i = 0; i < numb; ++i)
	{
		rbyte_data(pos, &dest[i]);
		#ifdef debug
		printf("Char read: %c at pos %d\n", dest[i], pos);
		#endif
		pos = global_fat[pos];
		if (pos == END_BLOCK) {
			printf("File finished\n");
			return i;
		}
	}
	return numb;
}

int write_file(char path[500], char str[])
{
	update();
	int free = fget_free();
	int tmp;
	if (free == -1) {
		perror("All blocks are full");
		return 1;
	}

	struct DIR_ENT dent;
	strcpy(dent.path, path);
	dent.size = strlen(str);
	dent.isdir = 0;
	dent.start = free;
	int md = mdget_free();
	global_dir[md] = dent;
	write_dir(&global_dir);

	for (int i = 0; i < strlen(str); ++i) {
		wbyte_data(free, str[i]);
		global_fat[free] = free;
		tmp = free;
		free = fget_free();
		global_fat[tmp] = free;
		#ifdef debug
		printf("Writen char %c at position %d Next in fat %d\n", str[i], tmp, free);
		#endif
	}

	global_fat[tmp] = END_BLOCK;
	#ifdef debug
	printf("Block %d assigned as last\n", tmp);
	#endif
	write_fat(&global_fat);
}

int md_find(char *path)
{
	for (int i = 0; i < sizeof(global_dir); ++i)
	{
		// printf("%s\n", global_dir[i].path);
		if (strcmp(global_dir[i].path, path) == 0) {
			// printf("%d\n", i);
			return i;
		}
	}
	perror("Could not find MetaData");
	return -1;
}

int mdget_free()
{
	for (int i = 0; i < sizeof(global_dir); ++i)
	{
		if (global_dir[i].path != "") {
			return i;
		}
	}
	return -1;
}

int fget_free()
{
	for (int i = 0; i < sizeof(FAT); ++i)
	{
		if (global_fat[i] == FREE_BLOCK) {
			return i;
		}
	}
	return -1;
}

int seek(int pos, struct DIR_ENT *dent)
{
	int curpos = dent->start;
	for (int i = 0; i < pos; ++i) {
		curpos = global_fat[curpos];
	}
	return curpos;
}

int update() 
{
	read_fat(&global_fat);
	read_dir(&global_dir);
}

int create_dir(char path[])
{
	struct DIR_ENT dent;
	dent.size = 0;
	dent.start = FREE_BLOCK;
	if (dent.start == -1) {
		return 1;
	}
	dent.isdir = 1;
	strcpy(dent.path, path);
	int pos = mdget_free();
	global_dir[pos] = dent;
	write_dir(&global_dir);
	return 0;
}

int rm_dir(char *path)
{
	int pos = md_find(path);
	if (!global_dir[pos].isdir) {
		perror("Not a directory");
	}
	struct DIR_ENT dent;
	global_dir[pos] = dent;
}

int check()
{
	if (check_fat()) {
		return 1;
	}
	if (check_dir()) {
		return 1;
	}
}

int check_fat()
{
	//If fat is not initialized in memory
	if (global_fat == NULL) {
		puts("Initializing FAT");
		if (write_fat(&global_fat)) {
			perror("Could not init FAT");
			return 1;
		}
	}
	return 0;
}

int check_dir()
{
	if (!global_dir) {
		puts("Initializing DIR");
		if (write_dir(&global_dir)) {
			perror("Could not init DIR");
			return 1;
		}
	}
	return 0;
}

int init()
{
    FAT fat;
    for (int i = 0; i < sizeof(FAT); ++i)
    {
        fat[i] = FREE_BLOCK;
    }
    if (write_fat(&fat)) {
        return 1;
    }
    DIR dir;
    if (write_dir(&dir)) {
        return 1;
    }
    DATA data;
    // _ means empty slot
    // memset(data, '_', sizeof(DATA));
    if (write_data(&data)) {
        return 1;
    }
    wbyte_data(777, 'L');
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
    char ch;
    if (rbyte_data(0, &ch)) {
        return 1;
    }
    printf("FAT: %d ", fat[55]);
    printf("DATA: %c\n", ch);
    return 0;
}