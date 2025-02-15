#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pg_attribute_parser.h"

int main(int argc, char** argv) {
    char* fname;
    if (argc > 1) {
        fname = argv[1];
    } else {
        fname = "example/1249";
    }
    FILE* pg_class_file = fopen(fname, "rb");
    if (pg_class_file == NULL) {
        return -1;
    }

    // массив для хранения сущностей
    AttributeItem* attributes = NULL;
    size_t attribute_count = 0;
    size_t attribute_capacity = 0;

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
           free(attributes);
           return -1;
        }

        // количество 4-байтных указателей на версии строк: от конца заголовка страницы до free space
        size_t num_items = (page_header.pd_lower - sizeof(PageHeaderData)) / sizeof(ItemIdData);

        // создание массива указателей на версии строк
        ItemIdData* items = (ItemIdData*)malloc(sizeof(ItemIdData) * num_items);
        if (items == NULL) {
            perror("Failed to allocate memory for items");
            fclose(pg_class_file);
            free(attributes);
            return -1;
        }

        // переход к началу массива ItemIdData и его чтение
        fseek(pg_class_file, current_offset + sizeof(PageHeaderData), SEEK_SET);
        if (fread(items, sizeof(ItemIdData), num_items, pg_class_file) != num_items) {
            perror("Failed to read items");
            free(items);
            fclose(pg_class_file);
            free(attributes);
            return -1;
        }

        for (size_t i = 0; i < num_items; i++) {
            if (!is_valid_item(&items[i], page_header.pd_upper)) {
                continue;
            }

            // переход к данным соответствующей строки и чтение Relation
            Attribute record;
            fseek(pg_class_file, current_offset + items[i].lp_off + TUPLEHEADERSIZE, SEEK_SET);
            if (fread(&record, sizeof(record), 1, pg_class_file) != 1) {
                perror("Failed to read record");
                free(items);
                fclose(pg_class_file);
                free(attributes);
                return -1;
            }

            if (attribute_count >= attribute_capacity) {
                attribute_capacity = attribute_capacity == 0 ? 1 : attribute_capacity * 2;
                AttributeItem *temp = (AttributeItem*) realloc(attributes, sizeof(AttributeItem) * attribute_capacity);
                if (temp == NULL) {
                    perror("Failed to reallocate memory for attributes");
                    free(items);
                    fclose(pg_class_file);
                    free(attributes);
                    return -1;
                }
                attributes = temp;
            }
            AttributeItem attribute;
            attribute.attrelid = record.attrelid;
            strncpy(attribute.attname, record.attname, sizeof(attribute.attname));
            attribute.atttypid = record.atttypid;
            attribute.attlen = record.attlen;
            attributes[attribute_count++] = attribute;
        }

        free(items);
        current_offset += PAGESIZE;
    }

    for (size_t i = 0; i < attribute_count; i++) {
        print_attribute(stdout, &attributes[i]);
        printf("\n");
    }

    fclose(pg_class_file);
    free(attributes);
    return 0;
}
