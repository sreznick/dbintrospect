#include "pg_class_parser.h"

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
