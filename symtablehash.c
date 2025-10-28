/*--------------------------------------------------------------------*/
/* symtablehash.c                                                     */
/* Author: Ryan Donoghue                                              */
/*--------------------------------------------------------------------*/
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "symtable.h"

/* Bucket count progression for expansion */
static const size_t auBucketCounts[] = {
    509, 1021, 2039, 4093, 8191, 16381, 32749, 65521
}; 

static const size_t numBucketCounts = sizeof(auBucketCounts)
/sizeof(auBucketCounts[0]);

/* Each binding is stored in a SymTableNode. SymTableNodes are 
linked to form a list within each bucket of the Hash Table. */
struct SymTableNode 
{
    /* The key of the binding. */
    char *pcKey;
    /* The value of the binding. */
    const void *pvValue;
    /* The address of the next binding in the bucket. Allows each 
    bucket to operate as individual linked lists. */
    struct SymTableNode *psNextNode;
};

/* A SymTable in the Hash Table implementation is an array of 
BUCKET_COUNT linked lists (Buckets) where bindings are stored in
nodes depending on their hash code. */
struct SymTable 
{
    /* Pointer to array of bucket pointers. */
    struct SymTableNode **ppsHashTable;
    /* Current number of buckets. */
    size_t uBucketCount; 
    /* Index for auBucketCounts array. */
    size_t uBucketIndex; 
    /* Number of bindings in the symbol table. */
    size_t uLength; 
};

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

/* Expand oSymTable to the next bucket count. Return 1 on success,
    0 on faliure (not enough memory). */
static int SymTable_expand(SymTable_T oSymTable) 
{
    struct SymTableNode **ppsNewBuckets;
    struct SymTableNode *psCurrentNode;
    struct SymTableNode *psNextNode;
    size_t uNewBucketCount;
    size_t uNewHash;
    size_t i;

    assert(oSymTable != NULL);


    /* Check if we can expand further */
    if (oSymTable->uLength >= auBucketCounts[numBucketCounts])
        {
        return 1;
        }
    /* Get new bucket count */
    uNewBucketCount = auBucketCounts[oSymTable->uBucketIndex + 1];
    
    ppsNewBuckets = (struct SymTableNode**)
        calloc(uNewBucketCount, sizeof(struct SymTableNode*));
    if (ppsNewBuckets == NULL)
    {
        return 0;
    }

    /* Rehash all existing bindings into new buckets */
    for (i = 0; i < oSymTable->uBucketCount; i++)
    {
        for (psCurrentNode = oSymTable->ppsHashTable[i];
            psCurrentNode != NULL;
            psCurrentNode = psNextNode)
        {
            /* Save next pointer before moving node */
            psNextNode = psCurrentNode->psNextNode;

            /* Rehash with new bucket count */
            uNewHash = SymTable_hash(psCurrentNode->pcKey, 
                uNewBucketCount);

            /* Add to front of new bucket */
            psCurrentNode->psNextNode = ppsNewBuckets[uNewHash];
            ppsNewBuckets[uNewHash] = psCurrentNode;
        }
    }
    /* Free old bucket array (but not the nodes - we moved them!) */
    free(oSymTable->ppsHashTable);

    /* Update to new buckets */
    oSymTable->ppsHashTable = ppsNewBuckets;
    oSymTable->uBucketCount = uNewBucketCount;
    oSymTable->uBucketIndex++;

    return 1;  /* Success */
}

SymTable_T SymTable_new(void) 
{
    SymTable_T oSymTable;

    oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
    if (oSymTable == NULL)
    {
        return NULL;
    }

    oSymTable->ppsHashTable = (struct SymTableNode**) 
        calloc(auBucketCounts[0], sizeof(struct SymTableNode*));
    if (oSymTable->ppsHashTable == NULL) 
    {
        free(oSymTable);
        return NULL; 
    }

    oSymTable->uBucketCount = auBucketCounts[0];
    oSymTable->uBucketIndex = 0;
    oSymTable->uLength = 0;

    return oSymTable; 
}

void SymTable_free(SymTable_T oSymTable) 
{
    struct SymTableNode *psCurrentNode;
    struct SymTableNode *psNextNode;
    size_t i; 

    assert(oSymTable != NULL); 

    for (i = 0; i < oSymTable->uBucketCount; i++)
    {
        if (oSymTable->ppsHashTable[i] != NULL) 
        {
            for (psCurrentNode = oSymTable->ppsHashTable[i];
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
    free(oSymTable->ppsHashTable); 
    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable) 
{
    return oSymTable->uLength; 
}

int SymTable_put(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) 
{
struct SymTableNode *psNewNode; 
struct SymTableNode *psCurrentNode;
size_t hash_code;

assert(oSymTable != NULL); 
assert(pcKey != NULL);

if(oSymTable->uLength >= oSymTable->uBucketCount)
{
    SymTable_expand(oSymTable); 
}

hash_code = SymTable_hash(pcKey, oSymTable->uBucketCount);

for (psCurrentNode = oSymTable->ppsHashTable[hash_code];
     psCurrentNode != NULL; 
     psCurrentNode = psCurrentNode->psNextNode)
{
    if (strcmp(psCurrentNode->pcKey, pcKey) == 0)
    {
        return 0;
    }
}

psNewNode = (struct SymTableNode*)malloc(sizeof(struct 
    SymTableNode)); 
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
psNewNode->psNextNode = oSymTable->ppsHashTable[hash_code]; 
oSymTable->ppsHashTable[hash_code] = psNewNode; 
oSymTable->uLength++; 
return 1; 

}

void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue) 
{
    struct SymTableNode *psCurrentNode;
    size_t hash_code; 
    
    const void *pvValueOld; 

    assert(oSymTable != NULL); 
    assert(pcKey != NULL);

    hash_code = SymTable_hash(pcKey, oSymTable->uBucketCount);
    
    for (psCurrentNode = oSymTable->ppsHashTable[hash_code];
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
    size_t hash_code; 

    assert(oSymTable != NULL);
    assert(pcKey != NULL); 

    hash_code = SymTable_hash(pcKey, oSymTable->uBucketCount);

    for (psCurrentNode = oSymTable->ppsHashTable[hash_code];
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
    size_t hash_code; 
    
    assert(oSymTable != NULL); 
    assert(pcKey != NULL); 

    hash_code = SymTable_hash(pcKey, oSymTable->uBucketCount);

    for (psCurrentNode = oSymTable->ppsHashTable[hash_code];
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
    struct SymTableNode *psFirstNode; 
    const void *pvRemovedValue; 
    size_t hash_code; 

    assert(oSymTable != NULL);
    assert(pcKey != NULL); 

    hash_code = SymTable_hash(pcKey, oSymTable->uBucketCount);
    psFirstNode = oSymTable->ppsHashTable[hash_code]; 

    if (psFirstNode != NULL && 
        strcmp(psFirstNode->pcKey, pcKey) == 0) 
        {
            pvRemovedValue = psFirstNode->pvValue;
            oSymTable->ppsHashTable[hash_code] = 
            psFirstNode->psNextNode;
            free(psFirstNode->pcKey);
            free(psFirstNode);
            oSymTable->uLength--; 
            return (void*)pvRemovedValue; 
        }

    /* Handle all other cases */
    for (psCurrentNode = psFirstNode;
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
        size_t extraApplied;
        size_t i; 
        
        assert(oSymTable != NULL);
        assert(pfApply != NULL);

        extraApplied = 0; 
    
     for (i = 0; i < oSymTable->uBucketCount; i++)
    {
        /* There is no need to continue traversing arrays if 
        all of the bindings have been modified. */
        if (oSymTable->uLength == extraApplied)
        {
            break; 
        }
        if (oSymTable->ppsHashTable[i] != NULL) 
        {
            for (psCurrentNode = oSymTable->ppsHashTable[i];
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