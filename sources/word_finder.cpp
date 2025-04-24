#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdint.h>

#include "hashtable.h"
#include "word_finder.h"

const size_t START_WORD_NUM = 500000;


static size_t getFileSize(FILE * file);

static char * loadFileToBuffer(const char * file_name, size_t * file_size);

static char ** divideBufferToStrings(char * buffer, size_t buf_size, size_t * word_count);


void loadWordsIntoTable(table_t * hashtab, const char * file_name)
{
    assert(hashtab);
    assert(file_name);


    size_t file_size = 0;
    char * text_buffer = loadFileToBuffer(file_name, &file_size);

    size_t word_count = 0;
    char ** words = divideBufferToStrings(text_buffer, file_size, &word_count);

    for (size_t word_index = 0; word_index < word_count; word_index++){
        uint32_t * data_ptr = (uint32_t *)tableLookup(hashtab, words[word_index]);

        if (data_ptr == NULL){
            uint32_t start_val = 1;
            tableInsert(hashtab, words[word_index], &start_val, sizeof(start_val));
        }
        else {
            (*data_ptr)++;
        }
    }

    free(words);
    free(text_buffer);
}

static char * loadFileToBuffer(const char * file_name, size_t * file_size)
{
    assert(file_name);

    FILE * text_file = fopen(file_name, "r");
    assert(text_file);

    *file_size = getFileSize(text_file);

    char * text_buffer = (char *)calloc(*file_size, sizeof(char));
    fread(text_buffer, sizeof(char), *file_size, text_file);

    fclose(text_file);

    return text_buffer;
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

                // printf("cap = %zu\n", capacity);
                // printf("idx = %zu\n", word_index);

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
