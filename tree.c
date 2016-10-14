#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct node
{
	char name[255];
	int child_size;
	struct node *children[1000];
};
struct node root = {.name = "/", .child_size = 0};

int split_string(char str[], char *arr[]);

int insert(char *path)
{
	if (strcmp(path, "/") == 0) {
		return 0;
	}
	//Split strings
	char *strs[strlen(path)];
	int count = split_string(path, strs);

	struct node *curr;
	curr = &root;
	for (int i = 0; i < count; ++i) {
		int boolean = -1;
		//Search in children
		for (int j = 0; j < curr->child_size; ++j) {
			int res = strcmp(strs[i], curr->children[j]->name);
			if (res == 0) {
				boolean = j;
			}
		}
		//Add child
		if (boolean == -1) {
			struct node *child;
			char tmp[strlen(strs[i])];
			strcpy(tmp, strs[i]);
			child  = (struct node*)malloc(sizeof(struct node));
			strcpy(child->name, strs[i]);
			curr->children[curr->child_size] = child;
			++curr->child_size;
			curr = child;
		} else {
			curr = curr->children[boolean];
		}
	}
	return 0;
}

int get_children(char *path, char **childs, int *number)
{
	if (strcmp(path, "/") == 0) {
		(*number) = 0;
		for (int i = 0; i < root.child_size; ++i) {
			childs[i] = root.children[i]->name;
			(*number)++;
		}
		return 0;
	}
	//Split strings
	char *strs[strlen(path)];
	int count = split_string(path, strs);

	struct node *curr;
	curr = &root;
	for (int i = 0; i < count; ++i) {
		int boolean = -1;
		//Search in children
		for (int j = 0; j < curr->child_size; ++j) {
			int res = strcmp(strs[i], curr->children[j]->name);
			if (res == 0) {
				boolean = j;
			}
		}
		if (boolean == -1) {
			puts("Path not in tree");
			return 1;
		}
		curr = curr->children[boolean];
	}
	(*number) = 0;
	for (int i = 0; i < curr->child_size; ++i) {
		childs[i] = curr->children[i]->name;
		(*number)++;
	}
	return 0;
}

int drop()
{   
	for (int i = 0; i < root.child_size; --root.child_size) {
		struct node *child = root.children[root.child_size - 1];
		free(child);
	}
}

int split_string(char str[], char **arr)
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
			#ifdef debug
			printf("split_string: '%s'\n", token);
			#endif
			arr[count] = (char*)malloc(strlen(token)*sizeof(char));
			strcpy(arr[count], token);
			++count;
		}
	}
	return count;
}