#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <string.h>
#include <errno.h>
#include "io.c"

static const char *filepath = "/file";
static const char *filename = "file";
static const char *filecontent = "I'm the content of the only file available there\n";

static int getattr_callback(const char *path, struct stat *stbuf) 
{
	int fd = open(file_path, O_RDONLY);
	if (fd < 0) {
		perror("BAD FD");
		return 1;
	}
	struct dir_ent *md = malloc(sizeof(struct dir_ent));
	int mdpos;
	int err = findMD(fd, path, md, &mdpos);
	if (md->isdir == FREE_BLOCK) {
		return -ENOENT;
	}
	if (md->isdir) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		return 0;
	} else {
		stbuf->st_mode = S_IFREG | 0777;
		stbuf->st_nlink = 1;
		stbuf->st_size = md->size*BLOCK_SIZE;
		return 0;
	}
}

static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler,
	off_t offset, struct fuse_file_info *fi) 
{
	(void) offset;
	(void) fi;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);

	char **children = malloc(sizeof(char)*255*100);
	int number;
	char *npath = malloc(strlen(path)*sizeof(char));
	strcpy(npath, path);
	get_children(npath, children, &number);

	for (int i = 0; i < number; ++i)
	{
		filler(buf, children[i], NULL, 0);
	}
	return 0;
}

static int open_callback(const char *path, struct fuse_file_info *fi) {
	puts("Im here");
	return 0;
}

static int read_callback(const char *path, char *buf, size_t size, off_t offset,
	struct fuse_file_info *fi) 
{
	puts("Trying to read file");
	int err = fRead(path, buf);
	printf("Buf has: %s\n", buf);
	if (err) {
		return size - 1;
	}
	return size;
}

static void *init_callback (struct fuse_conn_info *conn) {
	init();
}

static int write_callback (const char *path, const char *str, size_t size, off_t off,
struct fuse_file_info *fi)
{
	puts("im trying to write");
	fWrite(str, path);
	return size;
}

static int mkdir_callback (const char *path, mode_t mode)
{
	int err = mkDir(path);
	if (err) {
		return 1;
	}
	return 0;
}

static int create_callback (const char *path, mode_t mode, struct fuse_file_info *fi)
{
	puts("creating file");
	char *str = "";
	fi->fh = fWrite(str, path);
	return 0;
}

static struct fuse_operations fuse_example_operations = {
	.getattr = getattr_callback,
	.open = open_callback,
	.read = read_callback,
	.readdir = readdir_callback,
	.init = init_callback,
	.mkdir = mkdir_callback,
	.create = create_callback,
	.write = write_callback,
};

int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &fuse_example_operations, NULL);
}
