/*--------------------------------------------------------------------*/
/* symtablehash.c                                                     */
/* Author: Ryan Donoghue                                              */
/*--------------------------------------------------------------------*/
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "symtable.h"

enum {BUCKET_COUNT = 509};
struct SymTableHashNode 
{
    /* The key of the binding. */
    char *pcKey;
    /* The value of the binding. */
    const void *pvValue;
    /* The address of the next binding in the bucket. Allows each bucket
    to operate as individual linked lists. */
    struct SymTableHashNode *psNextNode;
};

/*--------------------------------------------------------------------*/

/* A SymTable in the Hash Table implementation is an array of 
BUCKET_COUNT linked lists (Buckets) where bindings are stored in
nodes depending on their hash code. */
struct SymTable 
{
    /* Pointer to array of bucket pointers. */
    struct SymTableHashNode **ppsBuckets;
    /* Current number of buckets. */
    size_t uBucketCount; 
    /* Number of bindings in the symbol table. */
    size_t uLength; 
};

/*--------------------------------------------------------------------*/

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive. */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void) 
{
    SymTable_T oSymTable;

    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if (oSymTable == NULL)
    {
        return NULL;
    }

    oSymTable->ppsBuckets = (struct SymTableHashNode**) 
        calloc(BUCKET_COUNT, sizeof(struct SymTableHashNode*));
    if (oSymTable->ppsBuckets == NULL) 
    {
        free(oSymTable);
        return NULL; 
    }

    oSymTable->uBucketCount = BUCKET_COUNT;
    oSymTable->uLength = 0;

    return oSymTable; 
}

/*--------------------------------------------------------------------*/

void SymTable_free(SymTable_T oSymTable) 
{
    struct SymTableHashNode *psCurrentNode;
    struct SymTableHashNode *psNextNode;
    size_t i; 

    assert(oSymTable != NULL); 

    for (i = 0; i < oSymTable->uBucketCount; i++)
    {
        /* There is no need to continue traversing arrays if there are
        no more bindings. */
        if (oSymTable->uLength == 0)
        {
            break; 
        }

        if (oSymTable->ppsBuckets[i] != NULL) 
        {
            for (psCurrentNode = oSymTable->ppsBuckets[i];
            psCurrentNode != NULL;
            psCurrentNode = psNextNode)
            {
                psNextNode = psCurrentNode->psNextNode;
                free(psCurrentNode->pcKey);
                free(psCurrentNode);
                oSymTable->uLength--; 
            }
        }
        
    }
    free(oSymTable->ppsBuckets); 
    free(oSymTable);
}

/*--------------------------------------------------------------------*/

size_t SymTable_getLength(SymTable_T oSymTable) 
{
    return oSymTable->uLength; 
}

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) 
{
struct SymTableHashNode *psNewNode; 
struct SymTableHashNode *psCurrentNode;
size_t hash_code;

assert(oSymTable != NULL); 
assert(pcKey != NULL);

hash_code = SymTable_hash(pcKey, oSymTable->uBucketCount);

for (psCurrentNode = oSymTable->ppsBuckets[hash_code];
     psCurrentNode != NULL; 
     psCurrentNode = psCurrentNode->psNextNode)
{
    if (strcmp(psCurrentNode->pcKey, pcKey) == 0)
    {
        return 0;
    }
}

psNewNode = (struct SymTableHashNode*)malloc(sizeof(struct SymTableHashNode)); 
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
psNewNode->psNextNode = oSymTable->ppsBuckets[hash_code]; 
oSymTable->ppsBuckets[hash_code] = psNewNode; 
oSymTable->uLength++; 
return 1; 

}

/*--------------------------------------------------------------------*/

void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) 
{
    struct SymTableHashNode *psCurrentNode;
    size_t hash_code; 
    
    const void *pvValueOld; 

    assert(oSymTable != NULL); 
    assert(pcKey != NULL);

    hash_code = SymTable_hash(pcKey, oSymTable->uBucketCount);
    
    for (psCurrentNode = oSymTable->ppsBuckets[hash_code];
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

/*--------------------------------------------------------------------*/

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) 
{
    struct SymTableHashNode *psCurrentNode;
    size_t hash_code; 

    assert(oSymTable != NULL);
    assert(pcKey != NULL); 

    hash_code = SymTable_hash(pcKey, oSymTable->uBucketCount);

    for (psCurrentNode = oSymTable->ppsBuckets[hash_code];
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

/*--------------------------------------------------------------------*/

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) 
{
    struct SymTableHashNode *psCurrentNode;
    const void *pvTargetValue; 
    size_t hash_code; 
    
    assert(oSymTable != NULL); 
    assert(pcKey != NULL); 

    hash_code = SymTable_hash(pcKey, oSymTable->uBucketCount);

    for (psCurrentNode = oSymTable->ppsBuckets[hash_code];
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

/*--------------------------------------------------------------------*/

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) 
{
    struct SymTableHashNode *psCurrentNode;
    struct SymTableHashNode *psPreviousNode = NULL; 
    struct SymTableHashNode *psFirstNode; 
    const void *pvRemovedValue; 
    size_t hash_code; 

    assert(oSymTable != NULL);
    assert(pcKey != NULL); 

    hash_code = SymTable_hash(pcKey, oSymTable->uBucketCount);
    psFirstNode = oSymTable->ppsBuckets[hash_code]; 

    for (psCurrentNode = psFirstNode;
        psCurrentNode != NULL; 
        psCurrentNode = psCurrentNode->psNextNode)
    {
        if (strcmp(psCurrentNode->pcKey, pcKey) == 0) 
        {
            if (psCurrentNode == psFirstNode) 
            {
                pvRemovedValue = psFirstNode->pvValue; 
                free(psCurrentNode->pcKey);
                free(psFirstNode);
                oSymTable->ppsBuckets[hash_code] = 
                psCurrentNode->psNextNode;
                oSymTable->uLength--; 
                return (void*)pvRemovedValue; 
            }
            /* Needed to avoid possibly null pointer dereference
             warning. psPreviousNode would already be established
             in the case where the else-if takes place. */
            else if (psPreviousNode != NULL)
            {
                pvRemovedValue = psCurrentNode->pvValue; 
                psPreviousNode->psNextNode = 
                psCurrentNode->psNextNode; 
                free(psCurrentNode->pcKey);
                free(psCurrentNode);
                oSymTable->uLength--; 
                return (void*)pvRemovedValue;
            }
        }
        psPreviousNode = psCurrentNode; 
    }
    return NULL; 
}

/*--------------------------------------------------------------------*/

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra) 
{
        struct SymTableHashNode *psCurrentNode;
        size_t extraApplied;
        size_t i; 
        
        assert(oSymTable != NULL);
        assert(pfApply != NULL);

    
     for (i = 0; i < oSymTable->uBucketCount; i++)
    {
        /* There is no need to continue traversing arrays if 
        all of the bindings have been modified. */
        if (oSymTable->uLength == extraApplied)
        {
            break; 
        }
        if (oSymTable->ppsBuckets[i] != NULL) 
        {
            for (psCurrentNode = oSymTable->ppsBuckets[i];
            psCurrentNode != NULL;
            psCurrentNode = psCurrentNode->psNextNode)
            {
                (*pfApply) ((void*)psCurrentNode->pcKey, 
                (void*)psCurrentNode->pvValue, ((void*)pvExtra)); 
                extraApplied++; 
            }
        }
        
    }
}