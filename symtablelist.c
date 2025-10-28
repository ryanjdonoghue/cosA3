/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Ryan Donoghue                                              */
/*--------------------------------------------------------------------*/
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "symtable.h"

/* Each binding is stored in a SymTableNode. SymTableNodes are linked
to form a list. */
struct SymTableNode 
{
    /* The Key of the binding. */
    char *pcKey;
    /* The value of the binding. */
    const void *pvValue;
    /* The address of the next SymTableNode. */
    struct SymTableNode *psNextNode;
    
}; 

/* A SymTable is a "dummy" node that points to the first 
SymTableNode. */
struct SymTable 
{
    /* The address of the first SymTableNode. */
    struct SymTableNode *psFirstNode;
    /* The length of the Symbol Table — its number of bindings. */
    size_t uLength;
}; 

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

void SymTable_free(SymTable_T oSymTable) 
{
    struct SymTableNode *psCurrentNode;
    struct SymTableNode *psNextNode;

    assert(oSymTable != NULL); 

    for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psNextNode)
    {
        psNextNode = psCurrentNode->psNextNode;
        free(psCurrentNode->pcKey);
        free(psCurrentNode);
    }
    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) {
    return oSymTable->uLength; 
}

int SymTable_put(SymTable_T oSymTable, 
    const char *pcKey, const void *pvValue) 
{
struct SymTableNode *psNewNode; 
struct SymTableNode *psCurrentNode;

assert(oSymTable != NULL); 
assert(pcKey != NULL);

for (psCurrentNode = oSymTable->psFirstNode;
     psCurrentNode != NULL; 
     psCurrentNode = psCurrentNode->psNextNode)
{
    if (strcmp(psCurrentNode->pcKey, pcKey) == 0)
    {
        return 0;
    }
}

psNewNode = (struct SymTableNode*)malloc(sizeof(struct SymTableNode)); 
if (psNewNode == NULL) 
{
    return 0;
}
psNewNode->pcKey = (char*)malloc(strlen(pcKey) + 1); 
if (psNewNode->pcKey == NULL)
{
    free(psNewNode);
    return 0; 
}
strcpy(psNewNode->pcKey, pcKey); 

psNewNode->pvValue = pvValue; 
psNewNode->psNextNode = oSymTable->psFirstNode; 
oSymTable->psFirstNode = psNewNode; 
oSymTable->uLength++; 
return 1; 

}

void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) 
{
    struct SymTableNode *psCurrentNode;
    
    const void *pvValueOld; 

    assert(oSymTable != NULL); 
    assert(pcKey != NULL);
    

    for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psCurrentNode->psNextNode)
    {
        if (strcmp(psCurrentNode->pcKey, pcKey) == 0) 
        {
            pvValueOld = psCurrentNode->pvValue;
            psCurrentNode->pvValue = pvValue;
            return (void*)pvValueOld; 
        }
    }
    return NULL; 

}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) 
{
    struct SymTableNode *psCurrentNode;

    assert(oSymTable != NULL);
    assert(pcKey != NULL) ; 

    for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psCurrentNode->psNextNode)
    {
        if (strcmp(psCurrentNode->pcKey, pcKey) == 0) 
        {
            return 1;
        }
    }
    return 0; 
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey)
{
    struct SymTableNode *psCurrentNode;
    const void *pvTargetValue; 
    
    assert(oSymTable != NULL); 
    assert(pcKey != NULL); 

    for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psCurrentNode->psNextNode)
    {
        if (strcmp(psCurrentNode->pcKey, pcKey) == 0) 
        {
            pvTargetValue = psCurrentNode->pvValue;
            return (void*)pvTargetValue; 
        }
    }
    return NULL; 
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) 
{
     struct SymTableNode *psCurrentNode;
    struct SymTableNode *psNodeToRemove;
    struct SymTableNode *psFirst;
    const void *pvRemovedValue; 

    assert(oSymTable != NULL);
    assert(pcKey != NULL); 

    /* Used a lot in first case, so I simplified the variable 
    name. */
    psFirst = oSymTable->psFirstNode;
    
    /* Handle if the binding to be removed is first in the 
    list. */
    if (psFirst != NULL && 
        strcmp(psFirst->pcKey, pcKey) == 0) 
        {
            pvRemovedValue = psFirst->pvValue;
            psFirst = psFirst->psNextNode;
            free(psFirst->pcKey);
            free(psFirst);
            oSymTable->uLength--; 
            return (void*)pvRemovedValue; 
        }

    /* Handle all other cases */
    for (psCurrentNode = psFirst;
        psCurrentNode != NULL; 
        psCurrentNode = psCurrentNode->psNextNode)
    {
        if (psCurrentNode->psNextNode != NULL && 
            strcmp(psCurrentNode->psNextNode->pcKey, pcKey) == 0)
        {
            psNodeToRemove = psCurrentNode->psNextNode; 
            pvRemovedValue = psNodeToRemove->pvValue; 
            psCurrentNode->psNextNode = psNodeToRemove->psNextNode;  
            free(psNodeToRemove->pcKey);
            free(psNodeToRemove);
            oSymTable->uLength--; 
            return (void*)pvRemovedValue;
        }
    }
    return NULL; 
}

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