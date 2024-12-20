#include "pg_class_parser.h"

void parse_pg_class(FILE* f, struct pg_class* const dst) {
    fread(dst, sizeof(*dst), 1, f);
    fseek(f, 0, SEEK_SET);
}

void dump_pg_class(FILE* f, const struct pg_class* const src) {
    fprintf(f, "pg_class:\n");
    fprintf(f, "oid: %u\n", src->oid);
    fprintf(f, "relname: %s\n", src->relname);
    fprintf(f, "relnamespace: %u\n", src->relnamespace);
    fprintf(f, "reltype: %u\n", src->reltype);
    fprintf(f, "reloftype: %u\n", src->reloftype);
    fprintf(f, "relowner: %u\n", src->relowner);
    fprintf(f, "relam: %u\n", src->relam);
    fprintf(f, "relfilenode: %u\n", src->relfilenode);
    fprintf(f, "reltablespace: %u\n", src->reltablespace);
    fprintf(f, "relpage: %d\n", src->relpage);
    fprintf(f, "reltuples: %f\n", src->reltuples);
    fprintf(f, "relallvisibl: %d\n", src->relallvisibl);
    fprintf(f, "reltoastrelid: %u\n", src->reltoastrelid);
    fprintf(f, "relhasindex: %d\n", src->relhasindex);
    fprintf(f, "relisshared: %u\n", src->relisshared);
    fprintf(f, "relpersistence: %d\n", src->relpersistence);
    fprintf(f, "relkind: %d\n", src->relkind);
    fprintf(f, "relnatts: %d\n", src->relnatts);
    fprintf(f, "relchecks: %d\n", src->relchecks);
    fprintf(f, "relhasrules: %d\n", src->relhasrules);
    fprintf(f, "relhastriggers: %d\n", src->relhastriggers);
    fprintf(f, "relhassubclass: %d\n", src->relhassubclass);
    fprintf(f, "relrowsecurity: %d\n", src->relrowsecurity);
    fprintf(f, "relforcerowsecurity: %d\n", src->relforcerowsecurity);
    fprintf(f, "relispopulated: %d\n", src->relispopulated);
    fprintf(f, "relreplident: %d\n", src->relreplident);
    fprintf(f, "relispartition: %d\n", src->relispartition);
    fprintf(f, "relrewrite: %u\n", src->relrewrite);
    fprintf(f, "relfrozenxid: %d\n", src->relfrozenxid);
    fprintf(f, "relminmxid: %d\n", src->relminmxid);
}
