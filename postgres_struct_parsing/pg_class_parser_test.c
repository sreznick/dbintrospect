#include <stdio.h>
#include <string.h>
#include "pg_class_parser.h"

int main(int argc, char** argv) {
    char* fname;
    if (argc > 1) {
        fname = argv[1];
    } else {
        fname = "example/1259";
    }
    FILE* pg_class_file = fopen(fname, "rb");
    if (pg_class_file == NULL) {
        return -1;
    }
    struct pg_class to_read;
    fseek(pg_class_file, MAGIC_OFFSET, SEEK_SET);
    parse_pg_class(pg_class_file, &to_read);
    char buf[32];
    long counter = 0;
    fclose(pg_class_file);
    dump_pg_class(stdout, &to_read);
}
