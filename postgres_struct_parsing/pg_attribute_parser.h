#pragma once

#include "../postgres_structs_descr/pg_attribute.h"
#include "../postgres_structs_descr/bufpage.h"

#include <stdio.h>

typedef struct AttributeItem {
    /* oid */
    Oid         attrelid;
    /* name of attribute */
    NameData	attname;
    /* OID of the instance in Catalog Class pg_type that
	 * defines the data type of this attribute (e.g. int4) */
    Oid			atttypid;
    /* copy of the typlen field from pg_type for this attribute */
    int16		attlen;

    /* add more fields if needed */
} AttributeItem;

bool is_valid_item(ItemIdData* item, LocationIndex pd_upper);
void print_attribute(FILE* f, const AttributeItem* const attribute);