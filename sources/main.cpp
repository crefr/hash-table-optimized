#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "hashtable.h"
#include "word_finder.h"

int main()
{
    table_t table = tableCtor(100);

    loadWordsIntoTable(&table, "test_data/data_to_store.txt");
    printf("LOADED!\n");

    while(1){
        char word_to_find[128] = "";
        scanf(" %s", word_to_find);

        uint32_t * search_result = (uint32_t *)tableLookup(&table, word_to_find);

        if (search_result == NULL)
            printf("There is no such word!\n");
        else
            printf("Number of '%s' = %u\n", word_to_find, *search_result);
    }

    tableDtor(&table);

    return 0;
}
