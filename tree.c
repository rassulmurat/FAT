#include "val.c"
#include <stdio.h>

struct node
{
	struct DIR_ENT *dent;

	struct node *children[100];
};
struct node root;

int add_node(struct node *parent, struct node *child)
{
	printf("%s\n", parent->dent->path);
	if (!parent->children[0]) {
		printf("Here is nothing\n");
	}
}