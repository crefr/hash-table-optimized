#ifndef WORD_FINDER_INCLUDED
#define WORD_FINDER_INCLUDED

#include "hashtable.h"

size_t loadWordsIntoTable(table_t * hashtab, const char * file_name);

double findWordsInTable(table_t * hashtab, const char * file_name, const size_t num_of_cycles);

#endif
