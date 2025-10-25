/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Ryan Donoghue                                              */
/*--------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------*/

size_t SymTable_getLength(SymTable_T oSymTable) {
    return oSymTable->uLength; 
}

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable, 
    const char *pcKey, const void *pvValue) 
{
struct SymTableNode *psNewNode; 

assert(oSymTable != NULL); 

psNewNode = (struct SymTableNode*)malloc(sizeof(struct SymTableNode)); 
if (psNewNode != NULL) 
{
    return 0;
}

psNewNode->pcKey = pcKey;
psNewNode->pvValue = pvValue; 
psNewNode->psNextNode = oSymTable->psFirstNode; 
oSymTable->psFirstNode = psNewNode; 
return 1; 

}

/*--------------------------------------------------------------------*/

void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) 
{
    struct SymTableNode *psCurrentNode;
    struct SymTableNode *psNextNode;
    const void *pvValueOld; 

    assert(oSymTable != NULL); 

    for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psNextNode)
    {
        if (psCurrentNode->pcKey == pcKey) {
            pvValueOld = psCurrentNode->pvValue;
            pvValue = psCurrentNode->pvValue;
            return (void*)pvValueOld; 
        }
    }
    return NULL; 

}

/*--------------------------------------------------------------------*/

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) 
{
    struct SymTableNode *psCurrentNode;
    struct SymTableNode *psNextNode;

    assert(oSymTable != NULL); 

    for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psNextNode)
    {
        if (psCurrentNode->pcKey == pcKey) {
            return 1;
        }
    }
    return 0; 
}

/*--------------------------------------------------------------------*/

void *SymTable_get(SymTable_T oSymTable, const char *pcKey)
{
    struct SymTableNode *psCurrentNode;
    const void *pvTargetValue; 
    
    assert(oSymTable != NULL); 

    for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psCurrentNode->psNextNode)
    {
        if (psCurrentNode->pcKey == pcKey) {
            pvTargetValue = psCurrentNode->pvValue;
            return (void*)pvTargetValue; 
        }
    }
    return NULL; 
}

/*--------------------------------------------------------------------*/

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) 
{
    struct SymTableNode *psCurrentNode;
    struct SymTableNode *psNextNode;
    struct SymTableNode *psLastNode = NULL; 
    const void *pvRemovedValue; 

    assert(oSymTable != NULL); 

    for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL; 
        psCurrentNode = psNextNode)
    {
        if (psCurrentNode->pcKey == pcKey) {
            if (psCurrentNode == oSymTable->psFirstNode) 
            {
                pvRemovedValue = oSymTable->psFirstNode->pvValue; 
                psNextNode = oSymTable->psFirstNode->psNextNode; 
                free(oSymTable->psFirstNode);
                oSymTable->psFirstNode = psNextNode; 
                return (void*)pvRemovedValue; 
            }
            else
            {
                pvRemovedValue = psCurrentNode->pvValue; 
                psLastNode->psNextNode = psNextNode; 
                free(psCurrentNode); 
                return (void*)pvRemovedValue;
            }
        }
        psLastNode = psCurrentNode; 
    }
}

/*--------------------------------------------------------------------*/

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra) 
{
        struct SymTableNode *psCurrentNode; 
        
        assert(oSymTable != NULL);
        assert(pfApply != NULL); 
        
     for (psCurrentNode = oSymTable->psFirstNode;
          psCurrentNode != NULL; 
          psCurrentNode = psCurrentNode->psNextNode)  
    { 
        (*pfApply) ((void*)psCurrentNode->pcKey, 
        (void*)psCurrentNode->pvValue, ((void*)pvExtra)); 
    } 
}