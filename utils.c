#include "pg_class_parser.h"
#include <stdlib.h>

bool is_valid_item(ItemIdData* item, LocationIndex pd_upper) {
    return item->lp_off > 0 && item->lp_off >= pd_upper;
}

bool is_relation_in_public_namespace(Oid relnamespace) {
    if (relnamespace == PUBLIC_NAMESPACE) {
        return true;
    }
    return false;
}

bool is_index(char relkind) {
    if (relkind == 'i') {
        return true;
    }
    return false;
}

// Reading file contents
char* read_file(const char *path) {
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