#pragma once

#include "postgres_structs_descr/pg_class.h"
#include "postgres_structs_descr/bufpage.h"
#include <stdio.h>

typedef struct Entity {
    /* oid */
    Oid         oid;
    /* class name */
    NameData    relname;

    /* add more fields if needed */
} Entity;

// на самом деле это не константы, нужно парсить таблицу pg_namespace
#define PG_CATALOG_NAMESPACE 11
#define PG_TOAST_NAMESPACE 99
#define INFORMATION_SCHEMA_NAMESPACE 100
#define PUBLIC_NAMESPACE 2200

bool is_valid_item(ItemIdData* item, LocationIndex pd_upper);
bool is_relation_in_public_namespace(Oid relnamespace);
bool is_index(char relkind);

Entity* get_btree_indexes(int *count);
