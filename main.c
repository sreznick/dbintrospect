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

#define _XOPEN_SOURCE 500 // Required for realpath
#include <limits.h> // Required for PATH_MAX
#include <unistd.h> // Required for realpath

static char db_path_prefix[256] = "";

static struct options {
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

	// Check if the path corresponds to one of indexes directories
    for (size_t i = 0; i < index_data.count; ++i) {
        char index_path[128];
        snprintf(index_path, sizeof(index_path), "%s__oid-%u", index_data.indexes[i].relname, index_data.indexes[i].oid);
        if (strcmp(path + 1, index_path) == 0) {
            stbuf->st_mode = S_IFDIR | 0755;
            stbuf->st_nlink = 2;
            return 0;
        }
    }

    // Check if the path corresponds to the index file (inside the directory)
    for (size_t i = 0; i < index_data.count; ++i) {
        char index_dir_path[128];
        snprintf(index_dir_path, sizeof(index_dir_path), "%s__oid-%u/%u", index_data.indexes[i].relname, index_data.indexes[i].oid, index_data.indexes[i].oid);
        if (strcmp(path + 1, index_dir_path) == 0) {
            stbuf->st_mode = S_IFREG | 0444;
            stbuf->st_nlink = 1;

            // Determine the file size
            char db_file_path[270];
            snprintf(db_file_path, sizeof(db_file_path), "%s/%u", db_path_prefix, index_data.indexes[i].oid);
            FILE *file = fopen(db_file_path, "r");
            if (file) {
                fseek(file, 0, SEEK_END);
                stbuf->st_size = ftell(file);
                fclose(file);
            } else {
                stbuf->st_size = 0;  // Default size if the file can't be opened
            }
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
        // Check if the path corresponds to one of indexes directories
        for (size_t i = 0; i < index_data.count; ++i) {
            char index_dir_path[128];
            snprintf(index_dir_path, sizeof(index_dir_path), "%s__oid-%u", index_data.indexes[i].relname, index_data.indexes[i].oid);
            if (strcmp(path + 1, index_dir_path) == 0) {
                // Add the index file inside the directory
                char index_file_name[16];
                snprintf(index_file_name, sizeof(index_file_name), "%u", index_data.indexes[i].oid);
                filler(buf, index_file_name, NULL, 0, FUSE_FILL_DIR_PLUS);
                return 0;
            }
        }
        return -ENOENT;
    }

	return 0;
}

static int btree_index_open(const char *path, struct fuse_file_info *fi)
{
	if ((fi->flags & O_ACCMODE) != O_RDONLY)
		return -EACCES;

	return 0;
}

static int btree_index_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	(void) fi;
	
    char *last_slash = strrchr(path, '/');
    unsigned int oid = atoi(last_slash + 1);

    // Find the index data for this OID
    Entity *index = NULL;
    for (size_t i = 0; i < index_data.count; ++i) {
        if (index_data.indexes[i].oid == oid) {
            index = &index_data.indexes[i];
            break;
        }
    }

    if (!index) {
        return -ENOENT; // Index not found
    }

    // Construct the path to the database file
    char db_file_path[270];
    snprintf(db_file_path, sizeof(db_file_path), "%s/%u", db_path_prefix, index->oid);

     // Check if the file exists
     struct stat file_stat;
     if (stat(db_file_path, &file_stat) != 0) {
         return -ENOENT;
     }

    // Read the file contents
    char *content = read_file(db_file_path);
    if (!content) {
        return -EIO; // Error reading file
    }

    // size_t len = strlen(content);
    size_t len = file_stat.st_size;
    fuse_log(FUSE_LOG_DEBUG, "btree_index_read: len = %zu\n", len);

    if (offset < len) {
        if (offset + size > len) {
            size = len - offset;
        }
        memcpy(buf, content + offset, size);
    } else {
        size = 0;
    }

    free(content);
    return size;
}

static const struct fuse_operations btree_index_oper = {
	.init       = btree_index_init,
	.getattr	= btree_index_getattr,
	.readdir	= btree_index_readdir,
	.open		= btree_index_open,
	.read		= btree_index_read,
};

static void show_help(const char *progname)
{
	printf("usage: %s [options] <mountpoint>\n\n", progname);
	printf("File-system specific options:\n");
    printf("\n");
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

    const char *relative_db_path = "database/";
    // Resolve the absolute path
    char absolute_db_path[PATH_MAX];
    if (realpath(relative_db_path, absolute_db_path) == NULL) {
        perror("realpath");
        strncpy(db_path_prefix, relative_db_path, sizeof(db_path_prefix) - 1);
        db_path_prefix[sizeof(db_path_prefix) - 1] = '\0';
    } else {
        strncpy(db_path_prefix, absolute_db_path, sizeof(db_path_prefix) - 1);
        db_path_prefix[sizeof(db_path_prefix) - 1] = '\0';
    }

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
