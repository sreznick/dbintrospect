#pragma once

#include "../postgres_structs_descr/pg_class.h"
#include "../postgres_structs_descr/bufpage.h"

#include <stdio.h>

typedef struct Entity {
    /* oid */
    Oid         oid;
    /* class name */
    NameData    relname;
    /* OID of namespace containing this class */
    Oid         relnamespace;
    /* r - table, i - index */
    char        relkind;

    /* add more fields if needed */
} Entity;

// на самом деле это не константы, нужно парсить таблицу pg_namespace
#define PG_CATALOG_NAMESPACE 11
#define PG_TOAST_NAMESPACE 99
#define INFORMATION_SCHEMA_NAMESPACE 100

bool is_valid_item(ItemIdData* item, LocationIndex pd_upper);
bool is_system_relation(Oid relnamespace);
bool is_table_or_index(char relkind);
void print_relation(FILE* f, const Relation* const src);
void print_page_header(FILE* f, const PageHeaderData* const page_header);
void print_entity(FILE* f, const Entity* const entity);