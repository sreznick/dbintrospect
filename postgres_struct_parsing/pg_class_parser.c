#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

    // массив для хранения сущностей
    Entity* entities = NULL;
    size_t entity_count = 0;
    size_t entity_capacity = 0;

    long file_size;
    fseek(pg_class_file, 0, SEEK_END);
    file_size = ftell(pg_class_file);
    fseek(pg_class_file, 0, SEEK_SET);

    long current_offset = 0;

    while (current_offset < file_size) {
        PageHeaderData page_header;
        fseek(pg_class_file, current_offset, SEEK_SET);
        if (fread(&page_header, sizeof(PageHeaderData), 1, pg_class_file) != 1) {
           if (feof(pg_class_file)) break;
           perror("Error reading page header");
           fclose(pg_class_file);
           free(entities);
           return -1;
        }

        // количество 4-байтных указателей на версии строк: от конца заголовка страницы до free space
        size_t num_items = (page_header.pd_lower - sizeof(PageHeaderData)) / sizeof(ItemIdData);

        // создание массива указателей на версии строк
        ItemIdData* items = (ItemIdData*)malloc(sizeof(ItemIdData) * num_items);
        if (items == NULL) {
            perror("Failed to allocate memory for items");
            fclose(pg_class_file);
            free(entities);
            return -1;
        }

        // переход к началу массива ItemIdData и его чтение
        fseek(pg_class_file, current_offset + sizeof(PageHeaderData), SEEK_SET);
        if (fread(items, sizeof(ItemIdData), num_items, pg_class_file) != num_items){
            perror("Failed to read items");
            free(items);
            fclose(pg_class_file);
            free(entities);
            return -1;
        }

        for (size_t i = 0; i < num_items; i++) {
            if (!is_valid_item(&items[i], page_header.pd_upper)) {
                continue;
            }

            // переход к данным соответствующей строки и чтение Relation
            Relation record;
            fseek(pg_class_file, current_offset + items[i].lp_off + TUPLEHEADERSIZE, SEEK_SET);
            if (fread(&record, sizeof(record), 1, pg_class_file) != 1) {
                perror("Failed to read record");
                free(items);
                fclose(pg_class_file);
                free(entities);
                return -1;
            }

            if (!is_system_relation(record.relnamespace) && is_table_or_index(record.relkind)) {
                if (entity_count >= entity_capacity) {
                    entity_capacity = entity_capacity == 0 ? 1 : entity_capacity * 2;
                    Entity *temp = (Entity*) realloc(entities, sizeof(Entity) * entity_capacity);
                    if (temp == NULL) {
                        perror("Failed to reallocate memory for entities");
                        free(items);
                        fclose(pg_class_file);
                        free(entities);
                        return -1;
                    }
                    entities = temp;
                }
                Entity entity;
                entity.oid = record.oid;
                strncpy(entity.relname, record.relname, sizeof(entity.relname));
                entity.relnamespace = record.relnamespace;
                entity.relkind = record.relkind;
                entities[entity_count++] = entity;
            }
        }

        free(items);
        current_offset += PAGESIZE;
    }

    for (size_t i = 0; i < entity_count; i++) {
        print_entity(stdout, &entities[i]);
        printf("\n");
    }

    fclose(pg_class_file);
    free(entities);
    return 0;
}
