#ifndef WORD_FINDER_INCLUDED
#define WORD_FINDER_INCLUDED

#include "hashtable.h"

size_t loadWordsIntoTable(table_t * hashtab, const char * file_name);

void findWordsInTable(table_t * hashtab, const char * file_name);

#endif
