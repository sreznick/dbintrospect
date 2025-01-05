#include "pg_class_parser.h"

bool is_valid_item(ItemIdData* item, LocationIndex pd_upper) {
    return item->lp_off > 0 && item->lp_off >= pd_upper;
}

bool is_system_relation(Oid relnamespace) {
    if (relnamespace == PG_CATALOG_NAMESPACE ||
        relnamespace == PG_TOAST_NAMESPACE || 
        relnamespace == INFORMATION_SCHEMA_NAMESPACE ) {
        return true;
    }
    return false;
}

bool is_relation_in_public_namespace(Oid relnamespace) {
    if (relnamespace == PUBLIC_NAMESPACE) {
        return true;
    }
    return false;
}

bool is_table_or_index(char relkind) {
    if (relkind == 'r' || relkind == 'i') {
        return true;
    }
    return false;
}

void print_relation(FILE* f, const Relation* const src) {
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

void print_page_header(FILE* f, const PageHeaderData* const page_header) {
    fprintf(f, "Metapage:\n");
    // SELECT * FROM page_header(get_raw_page('pg_class', 0));
    fprintf(f, "lsn: %x/%x\n", page_header->pd_lsn.xlogid, page_header->pd_lsn.xrecoff);
    fprintf(f, "checksum: %d\n", page_header->pd_checksum);
    fprintf(f, "flags: %d\n", page_header->pd_flags);
    fprintf(f, "lower: %d\n", page_header->pd_lower);
    fprintf(f, "upper: %d\n", page_header->pd_upper);
    fprintf(f, "special: %d\n", page_header->pd_special);
    fprintf(f, "pagesize: %d\n", PAGESIZE);
    fprintf(f, "version: %d\n", page_header->pd_pagesize_version - PAGESIZE);
    fprintf(f, "prune_xid: %d\n", page_header->pd_prune_xid);
}

void print_entity(FILE* f, const Entity* const entity) {
    fprintf(f, "oid: %u\n", entity->oid);
    fprintf(f, "relname: %s\n", entity->relname);
    fprintf(f, "relnamespace: %u\n", entity->relnamespace);
    fprintf(f, "relkind: %c\n", entity->relkind);
}