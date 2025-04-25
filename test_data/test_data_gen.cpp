#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

typedef struct {
    const char * word;
    size_t len;
} word_t;


typedef struct {
    word_t * words;
    size_t word_count;
    size_t capacity;
} word_list_t;


const char * TEXT1_FILE_NAME  = "shakespeare.txt";
const size_t WORDS_FROM_TEXT1 = 5000000;

const char * TEXT2_FILE_NAME  = "war-and-peace.txt";
const size_t WORDS_FROM_TEXT2 = 5000000;

const char * STORE_FILE_NAME = "data_to_store.txt";
const char *  FIND_FILE_NAME = "data_to_find.txt";

const size_t START_WORD_CAP  = 1024;

const size_t MAX_WORD_LEN = 15;


word_list_t wordListCtor(size_t start_cap);

void wordListDtor(word_list_t * word_list);

void wordListReallocIfNeed(word_list_t * word_list);

word_list_t divideIntoWords(const char * buffer, size_t buf_size);

size_t readFileToBuffer(const char * file_name, char ** buffer);

void randomDumpToFile(FILE * file, word_list_t * word_list, size_t max_word_len, size_t word_num);

size_t getFileSize(FILE * file);

void dumpWordListToFile(FILE * file, word_list_t * word_list, size_t max_word_len);


int main()
{
    char * text1 = NULL;
    char * text2 = NULL;

    size_t text1_size = readFileToBuffer(TEXT1_FILE_NAME, &text1);
    size_t text2_size = readFileToBuffer(TEXT2_FILE_NAME, &text2);

    FILE * file_to_store = fopen(STORE_FILE_NAME, "w");
    FILE * file_to_find  = fopen( FIND_FILE_NAME, "w");

    word_list_t text1_word_list = divideIntoWords(text1, text1_size);
    word_list_t text2_word_list = divideIntoWords(text2, text2_size);

    dumpWordListToFile(file_to_store, &text1_word_list, MAX_WORD_LEN);

    randomDumpToFile(file_to_find, &text1_word_list, MAX_WORD_LEN, WORDS_FROM_TEXT1);
    randomDumpToFile(file_to_find, &text2_word_list, MAX_WORD_LEN, WORDS_FROM_TEXT2);

    wordListDtor(&text1_word_list);

    fclose(file_to_store);
    fclose(file_to_find);

    free(text1);
    free(text2);
}

size_t readFileToBuffer(const char * file_name, char ** buffer)
{
    assert(file_name);

    FILE * file = fopen(file_name, "r");

    size_t file_size = getFileSize(file);
    *buffer = (char *)calloc(file_size, sizeof(char));

    fread(*buffer, sizeof(char), file_size, file);

    fclose(file);

    return file_size;
}

word_list_t wordListCtor(size_t start_cap)
{
    word_list_t words = {};

    words.capacity = start_cap;
    words.word_count = 0;
    words.words = (word_t *)calloc(start_cap, sizeof(*(words.words)));

    return words;
}

void wordListDtor(word_list_t * word_list)
{
    assert(word_list);

    free(word_list->words);
}

void wordListReallocIfNeed(word_list_t * word_list)
{
    assert(word_list);

    if (word_list->word_count >= word_list->capacity - 10){
        size_t new_cap = word_list->capacity * 2;
        word_list->capacity = new_cap;
        word_list->words = (word_t *)realloc(word_list->words, sizeof(word_t) * new_cap);
    }
}

void dumpWordListToFile(FILE * file, word_list_t * word_list, size_t max_word_len)
{
    assert(file);
    assert(word_list);

    for (size_t word_index = 0; word_index < word_list->word_count; word_index++){
        fwrite(word_list->words[word_index].word, sizeof(char), word_list->words[word_index].len, file);
        fputc('\n', file);
    }
}

void randomDumpToFile(FILE * file, word_list_t * word_list, size_t max_word_len, size_t word_num)
{
    assert(file);
    assert(word_list);

    size_t word_list_len = word_list->word_count;

    for (size_t word_index = 0; word_index < word_num; word_index++){
        word_t * word_to_write = word_list->words + (rand() % word_list_len);

        fwrite(word_to_write->word, sizeof(char), word_to_write->len, file);
        fputc('\n', file);
    }
}

word_list_t divideIntoWords(const char * buffer, size_t buf_size)
{
    assert(buffer);

    word_list_t word_list = wordListCtor(START_WORD_CAP);

    size_t current_word_len = 0;
    bool last_is_alpha = false;

    for (const char * cur_char = buffer; cur_char < buffer + buf_size; cur_char++){
        if (isalpha(*cur_char)){
            if (last_is_alpha){
                current_word_len++;
            }

            else {
                wordListReallocIfNeed(&word_list);
                word_list.words[word_list.word_count].word = cur_char;

                last_is_alpha = true;
            }
        }
        else {
            if (last_is_alpha){
                word_list.words[word_list.word_count].len = current_word_len + 1;
                current_word_len = 0;
                word_list.word_count++;
            }

            last_is_alpha = false;
        }
    }

    return word_list;
}

size_t getFileSize(FILE * file)
{
    assert(file);

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return size;
}
