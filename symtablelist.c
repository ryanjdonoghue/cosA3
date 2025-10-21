/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Ryan Donoghue                                              */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "symtable.h"

/*--------------------------------------------------------------------*/

/* Each binding is stored in a SymTableNode. SymTableNodes are linked
to form a list. */
struct SymTableNode 
{
    char *pcKey;
    const void *pvValue;
    struct SymTableNode *psNextNode;
    
}; 

/*--------------------------------------------------------------------*/

/* A SymTable is a "dummy" node that points to the first 
SymTableNode. */

struct SymTable 
{
    struct SymTableNode *psFirstNode;
    size_t uLength;
}; 

/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void)
{
    SymTable_T oSymTable;

    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if (oSymTable == NULL) {
        return NULL;
    }
    oSymTable->psFirstNode = NULL;
    oSymTable->uLength = 0;
    return oSymTable;
}

/*--------------------------------------------------------------------*/

void SymTable_free(SymTable_T oSymTable) {
    struct SymTableNode *psCurrentNode;
    struct SymTableNode *psNextNode;

    assert(oSymTable != NULL); 

    for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psNextNode)
    {
        psNextNode = psCurrentNode->psNextNode;
        free(psCurrentNode); 
    }
    free(oSymTable); 
}

size_t SymTable_getLength(SymTable_T oSymTable) {
    return oSymTable->uLength; 
}

