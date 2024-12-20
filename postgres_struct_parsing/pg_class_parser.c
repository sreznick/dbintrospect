#include "pg_class_parser.h"

void parse_pg_class(FILE* f, struct pg_class* dst) {
    fread(dst, sizeof(*dst), 1, f);
    fseek(f, 0, SEEK_SET);
}
