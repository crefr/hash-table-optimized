#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#include "hashtable.h"
#include "word_finder.h"

const size_t TABLE_SIZE = 2047;

const char * const FILE_FOR_STORE_NAME = "test_data/data_to_store.txt";
const char * const FILE_FOR_FIND_NAME  = "test_data/data_to_find.txt";

const size_t NUM_OF_CYCLES = 2;


#define PRINT_TIME(fmt_str, function)                                                                                    \
do {                                                                                                            \
    struct timespec calc_start = {};                                                                            \
    struct timespec calc_end = {};                                                                              \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &calc_start);                                                                \
    function;                                                                                                   \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &calc_end);                                                                  \
    double calc_time = 1000 * (calc_end.tv_sec - calc_start.tv_sec) + (calc_end.tv_nsec - calc_start.tv_nsec) / 1e6;   \
    printf(fmt_str, calc_time);                                                                              \
} while(0)


int main(int argc, char ** argv)
{
    table_t table = tableCtor(TABLE_SIZE);
    printf("table size = %zu\n\n", TABLE_SIZE);

    size_t words_loaded = loadWordsIntoTable(&table, FILE_FOR_STORE_NAME);
    printf("LOADED %zu different words!\n", words_loaded);

    printf("words per bucket = %f\n\n", (float)words_loaded / (float)TABLE_SIZE);

    if (argc > 1 && strcmp("-t", argv[1]) == 0){
        printf("started finding...\n");

        PRINT_TIME("finding lasted %lf ms\n",
        findWordsInTable(&table, FILE_FOR_FIND_NAME, NUM_OF_CYCLES));
    }
    else {
        char word_to_find[128] = "";
        scanf(" %s", word_to_find);

        while(strcmp(word_to_find, "-q") != 0){
            uint32_t * search_result = (uint32_t *)tableLookup(&table, word_to_find);

            if (search_result == NULL)
                printf("There is no such word!\n");
            else
                printf("Number of '%s' = %u\n", word_to_find, *search_result);

            scanf(" %s", word_to_find);
        }

    }

    tableDtor(&table);

    return 0;
}
