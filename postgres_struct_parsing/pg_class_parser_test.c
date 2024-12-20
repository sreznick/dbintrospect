#include <stdio.h>
#include <string.h>
// #include "pg_class_parser.h"

int main() {
    FILE* pg_class_file = fopen("example/1259_13797", "rb");
    // struct pg_class to_read;
    // parse_pg_class(pg_class_file, &to_read);
    FILE* chardump = fopen("chardump.txt", "w");
    char buf[32];
    long counter = 0;
    while(fread(buf, 1, 32, pg_class_file) == 32) {
        for (int i = 0; i < 32; ++i) {
            if (!((buf[i] >= 'a' && buf[i] <= 'z') || (buf[i] >= 'A' && buf[i] <= 'Z') || (buf[i] >= '0' && buf[i] <= '9') || (buf[i] == '_'))) {
                buf[i] = '.';
            }
        }
        buf[31] = 0;
        if (strstr(buf, "pg_class.") != NULL) {
            printf("%ld %s\n", counter, buf);
        }
        fwrite(buf, 1, 1, chardump);
        fseek(pg_class_file, -32, SEEK_CUR);
        int t;
        fread(&t, sizeof(int), 1, pg_class_file);
        fseek(pg_class_file, -sizeof(int) + 1, SEEK_CUR);

        if (t == 1259) {
            printf("1259 at %ld\n", counter);
        }
        if (counter % 1024 == 1) {
            printf("I am alive\n");
        }
        if (counter % 128 == 0) {
            fwrite("\n", 1, 1, chardump);
        }
        if (counter % 8192 == 0) {
            printf("\n");
            fwrite("\n", 1, 1, chardump);
        }
        counter += 1;
    }
    fclose(chardump);
    fclose(pg_class_file);
    // printf("Oid: %d, relname: %s\n", to_read.oid, to_read.relname);
}
