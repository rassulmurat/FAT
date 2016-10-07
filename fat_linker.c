#include "io_basic.c"
#include "fat_linker.h"

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
	update();
	for (int i = 0; i < sizeof(global_dir); ++i)
	{
		if (strcmp(global_dir[i].path, path) == 0) {
			return i;
		}
	}
	perror("Could not find MetaData");
	return -1;
}

int md_get(int pos, struct DIR_ENT *md)
{
	if (pos < sizeof(global_dir)) {
		md = &global_dir[pos];
		return 0;
	}
	perror("MD out of range");
	return 1;
}

int mdget_free()
{
	for (int i = 0; i < sizeof(global_dir); ++i)
	{
		if (global_dir[i].path[0] == '\0') {
			#ifdef debug
			printf("Empty MD cell at %d\n", i);
			#endif
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
			#ifdef debug
			printf("Empty FAT cell at %d\n", i);
			#endif
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

int find_children(char *path, struct DIR_ENT *children[])
{
	char *arrtmp[sizeof(path)];
	int tmp = split_string(path, arrtmp);
	char *arrm[tmp];
	cpyarr(arrm, arrtmp, tmp);
	int added = 0;
	for (int i = 0; i < sizeof(global_dir); ++i)
	{
		if (global_dir[i].path[0] == '\0') {
			break;
		}
		// #ifdef debug
		printf("'%s id: %d'\n", global_dir[i].path, i);
		// #endif
        char *arrtmp2[sizeof(global_dir[i].path)];
		tmp = split_string(global_dir[i].path, arrtmp2);
		// printf("tmp %d\n", tmp);
		char *arrglob[tmp];
		cpyarr(arrglob, arrtmp, tmp);
        int nelm = sizeof(arrm)/8;
        int nelgl = sizeof(arrglob)/8;
		if (nelm + 1 == nelgl) {
			int state = 0;
			for (int i1 = 0; i1 < sizeof(arrm)/8; ++i1)
			{
				if (strcmp(arrm[i1], arrglob[i1]) == 0) {
					state = 1;
				} else {
					state = 0;
					break;
				}
			}
			if (state) {
				#ifdef debug
				printf("Child added, path: %s %d\n", global_dir[i].path, i);
				#endif
				children[added] = &global_dir[i];
				++added;
			}
		}
	}
	return added;
}

int cpyarr(char *dst[], char *from[], int size)
{
	for (int i = 0; i < size; ++i)
	{
		dst[i] = from[i];
	}
}

int split_string(char str[], char *arr[])
{
	const char *s = "/";
    char *token;
    char tmp[strlen(str)];
    strcpy(tmp, str);
	token = strtok(tmp, s);
    arr[0] = token;
    int count = 1;

    while (token != NULL) {
        token = strtok(NULL, s);
        if (token != NULL) {
        	// #ifdef debug
	        printf("split_string: '%s'\n", token);
	        // #endif
        	arr[count] = token;
        	++count;
        }
    }
    // printf("%d\n", count);
    return count;
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