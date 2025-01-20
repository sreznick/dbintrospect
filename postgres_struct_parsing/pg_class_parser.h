#pragma once

#include "pg_class.h"

#include <stdio.h>

void parse_pg_class(FILE* f, struct pg_class* const dst);
void dump_pg_class(FILE* f, const struct pg_class* const src);
