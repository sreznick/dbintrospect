#define FUSE_USE_VERSION 31

#include "pg_class_parser.h"
#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>

static struct options {
	const char *filename;
	const char *contents;
	int show_help;
} options;

#define OPTION(t, p)                           \
    { t, offsetof(struct options, p), 1 }
static const struct fuse_opt option_spec[] = {
	OPTION("-h", show_help),
	OPTION("--help", show_help),
	FUSE_OPT_END
};

// Global structure to hold the index data
typedef struct {
    Entity *indexes;
    size_t count;
} IndexData;

static IndexData index_data = {NULL, 0};

static void *btree_index_init(struct fuse_conn_info *conn,
			struct fuse_config *cfg)
{
	(void) conn;
	cfg->kernel_cache = 1;
	return NULL;
}

static int btree_index_getattr(const char *path, struct stat *stbuf,
			 struct fuse_file_info *fi)
{
	(void) fi;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

	// Check if the path corresponds to one of indexes
    for (size_t i = 0; i < index_data.count; ++i) {
        char index_path[128];
        snprintf(index_path, sizeof(index_path), "%s__oid-%u", index_data.indexes[i].relname, index_data.indexes[i].oid);
        if (strcmp(path + 1, index_path) == 0) {
            stbuf->st_mode = S_IFDIR | 0755;
            stbuf->st_nlink = 2;
            return 0;
        }
    }

	return -ENOENT;
}

static int btree_index_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi,
			 enum fuse_readdir_flags flags)
{
	(void) offset;
	(void) fi;
	(void) flags;

	filler(buf, ".", NULL, 0, FUSE_FILL_DIR_PLUS);
    filler(buf, "..", NULL, 0, FUSE_FILL_DIR_PLUS);

	if (strcmp(path, "/") == 0) {
        // Add each index as a directory
        for (size_t i = 0; i < index_data.count; ++i) {
            char index_name[128];
            snprintf(index_name, sizeof(index_name), "%s__oid-%u", index_data.indexes[i].relname, index_data.indexes[i].oid);
            filler(buf, index_name, NULL, 0, FUSE_FILL_DIR_PLUS);
        }
    } else {
        return -ENOENT;
    }

	return 0;
}

static const struct fuse_operations btree_index_oper = {
	.init       = btree_index_init,
	.getattr	= btree_index_getattr,
	.readdir	= btree_index_readdir,
	// .open		= btree_index_open,
	// .read		= btree_index_read,
};

static void show_help(const char *progname)
{
	printf("usage: %s [options] <mountpoint>\n\n", progname);
	printf("File-system specific options:\n");
    printf("\n");
}

// Reading file contents
static char* read_file(const char *path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(file_size + 1);
    if (!buffer) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';

    fclose(file);
    return buffer;
}

int main(int argc, char *argv[])
{
	int ret;
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    // Initialize index data
	int count;
	index_data.indexes = get_btree_indexes(&count);
	index_data.count = count;

	/* Parse options */
	if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1)
		return 1;

	/* When --help is specified, first print our own file-system
	   specific help text, then signal fuse_main to show
	   additional help (by adding `--help` to the options again)
	   without usage: line (by setting argv[0] to the empty
	   string) */
	if (options.show_help) {
		show_help(argv[0]);
		assert(fuse_opt_add_arg(&args, "--help") == 0);
		args.argv[0][0] = '\0';
	}

	ret = fuse_main(args.argc, args.argv, &btree_index_oper, NULL);

	// Clean up allocated memory
	if (index_data.indexes != NULL) {
        free(index_data.indexes);
        index_data.indexes = NULL;
    }

	fuse_opt_free_args(&args);
	return ret;
}
