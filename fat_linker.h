int read_file(char *path, int numb, char *dest, int pointer_pos);
int update();
int write_file(char path[500], char str[]);
int md_find(char *path);
int seek(int pos, struct DIR_ENT *dent);
int fget_free();
int check_fat();
int check ();
int check_dir();
int mdget_free();
int rm_dir(char *path);
int split_string(char str[], char *arr[]);
int cpyarr(char *dst[], char *from[], int size);
int initfiles();
char *str_add(char *str1, char *str2);