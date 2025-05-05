#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "hashtable.h"
#include "word_finder.h"

typedef struct {
    char *  buffer;
    char ** words;

    size_t word_count;
} word_list_t;

const size_t START_WORD_NUM = 5000000;


static size_t getFileSize(FILE * file);

static word_list_t wordListCtor(const char * file_name);

static char ** divideBufferToStrings(char * buffer, size_t buf_size, size_t * word_count);

static void wordListAlign(word_list_t * word_list);

static void wordListDtor(word_list_t * word_list);


static void * aligned_realloc(void * old_ptr, size_t old_size, size_t new_size, size_t alignment){
    // printf("reallocating...\n");

    void * new_ptr = aligned_alloc(alignment, new_size);

    size_t copy_size = old_size < new_size ? old_size : new_size;

    memcpy(new_ptr, old_ptr, copy_size);
    free(old_ptr);

    return new_ptr;
}


size_t loadWordsIntoTable(table_t * hashtab, const char * file_name)
{
    assert(hashtab);
    assert(file_name);

    word_list_t word_list = wordListCtor(file_name);
    wordListAlign(&word_list);

    char ** words = word_list.words;

    size_t word_count = word_list.word_count;
    size_t words_loaded = 0;

    for (size_t word_index = 0; word_index < word_count; word_index++){
        // printf("word = %s\n", words[word_index]);
        uint32_t * data_ptr = (uint32_t *)tableLookup(hashtab, words[word_index]);

        if (data_ptr == NULL){
            uint32_t start_val = 1;
            words_loaded++;
            tableInsert(hashtab, words[word_index], &start_val, sizeof(start_val));
        }
        else {
            (*data_ptr)++;
        }
    }

    wordListDtor(&word_list);

    return words_loaded;
}


double findWordsInTable(table_t * hashtab, const char * file_name, const size_t num_of_cycles)
{
    assert(hashtab);
    assert(file_name);

    word_list_t word_list = wordListCtor(file_name);
    wordListAlign(&word_list);

    char ** words = word_list.words;
    size_t word_count = word_list.word_count;

    size_t success_finds = 0;

    struct timespec find_start = {};
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &find_start);

    for (size_t cycle_index = 0; cycle_index < num_of_cycles; cycle_index++){
        for (size_t word_index = 0; word_index < word_count; word_index++){
            uint32_t * search_result = (uint32_t *)tableLookup(hashtab, words[word_index]);

            if (search_result != NULL)
                success_finds++;
        }
    }

    struct timespec find_end = {};
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &find_end);

    printf("Successfully found %zu words out of %zu\n", success_finds, word_count);

    wordListDtor(&word_list);

    double find_time = 1000 * (find_end.tv_sec - find_start.tv_sec) + (find_end.tv_nsec - find_start.tv_nsec) / 1e6;

    return find_time;
}


static word_list_t wordListCtor(const char * file_name)
{
    assert(file_name);

    FILE * text_file = fopen(file_name, "r");
    assert(text_file);

    size_t file_size = getFileSize(text_file);

    char * text_buffer = (char *)calloc(file_size, sizeof(char));
    fread(text_buffer, sizeof(char), file_size, text_file);

    fclose(text_file);

    size_t word_count = 0;
    char ** words = divideBufferToStrings(text_buffer, file_size, &word_count);

    word_list_t word_list = {
        .buffer = text_buffer,
        .words  = words,
        .word_count = word_count
    };

    return word_list;
}


static void wordListDtor(word_list_t * word_list)
{
    free(word_list->buffer);
    free(word_list->words);
}


static void wordListAlign(word_list_t * word_list)
{
    assert(word_list);
    assert(word_list->buffer);
    assert(word_list->words);

    size_t word_count = word_list->word_count;
    char ** words = word_list->words;

    size_t capacity = word_count * NAME_ALIGNMENT;
    char * aligned_buf = (char *)aligned_alloc(NAME_ALIGNMENT, capacity);

    size_t aligned_index = 0;

    for (size_t word_index = 0; word_index < word_count; word_index++){
        size_t word_len = strlen(words[word_index]) + 1; // with '\0'

        if (aligned_index * NAME_ALIGNMENT + word_len + NAME_ALIGNMENT + 1 >= capacity){
            aligned_buf = (char *)aligned_realloc(aligned_buf, capacity, capacity * 2, NAME_ALIGNMENT);
            capacity *= 2;
        }
        size_t cur_index = aligned_index * NAME_ALIGNMENT;
        memcpy(aligned_buf + cur_index, words[word_index], word_len);

        // pointer = index ???
        words[word_index] = (char *)cur_index;

        cur_index += word_len;
        size_t shift = word_len % NAME_ALIGNMENT;

        if (shift)
            memset(aligned_buf + cur_index, '\0', NAME_ALIGNMENT - shift);

        aligned_index += (word_len / NAME_ALIGNMENT) + ((shift) ? 1 : 0);
    }

    // translating pointers to pointers (not indexes)
    for (size_t word_index = 0; word_index < word_count; word_index++){
        words[word_index] = aligned_buf + (size_t)words[word_index];
    }

    free(word_list->buffer);
    word_list->buffer = aligned_buf;

    // for (size_t word_index = 0; word_index < word_count; word_index++){
    //     printf("%p : %s\n", words[word_index], words[word_index]);
    // }
}


static char ** divideBufferToStrings(char * buffer, size_t buf_size, size_t * word_count)
{
    assert(buffer);

    char ** words = (char **)calloc(START_WORD_NUM, sizeof(*words));
    size_t word_index = 0;
    size_t capacity   = START_WORD_NUM;

    bool last_is_alpha = false;

    for (char * cur_char = buffer; cur_char < buffer + buf_size; cur_char++){
        if (isalpha(*cur_char)){
            if (!last_is_alpha){
                if (word_index == capacity - 1){
                    capacity *= 2;
                    words = (char **)realloc(words, capacity * sizeof(*words));
                }

                words[word_index] = cur_char;
                word_index++;

                last_is_alpha = true;
            }
        }
        else {
            *cur_char = '\0';
            last_is_alpha = false;
        }
    }

    *word_count = word_index;

    return words;
}



static size_t getFileSize(FILE * file)
{
    assert(file);

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return size;
}
