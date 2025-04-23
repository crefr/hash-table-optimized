#include <stdio.h>
#include <stdlib.h>

#include "hashtable.h"

int main()
{
    table_t table = tableCtor(100);

    int data = 52;
    tableInsert(&table, "amogus", &data, sizeof(data));

    printf("amogus = %p\n", tableLookup(&table, "amogus"));

    tableDtor(&table);

    return 0;
}
