/* Expand the hash table to the next bucket count. Return 1 on success,
    0 on faliure (not enough memory). */
static int SymTable_expand(SymTable_T oSymTable) 
{
    struct SymTableHashNode **ppsNewBuckets;
    struct SymTableHashNode *psCurrentNode;
    struct SymTableHashNode *psNextNode;
    size_t uNewBucketCount;
    size_t uNewHash;
    size_t i;

    assert(oSymTable != NULL);


    /* Check if we can expand further */
    if (oSymTable->uLength >= auBucketCounts[NUM_BUCKET_COUNTS])
        {
        return 1;
        }
    /* Get new bucket count */
    uNewBucketCount = auBucketCounts[oSymTable->uBucketIndex + 1];
    
    ppsNewBuckets = (struct SymTableHashNode**)
        calloc(uNewBucketCount, sizeof(struct SymTableHashNode*));
    if (ppsNewBuckets == NULL)
    {
        return 0;
    }

    /* Rehash all existing bindings into new buckets */
    for (i = 0; i < oSymTable->uBucketCount; i++)
    {
        for (psCurrentNode = oSymTable->ppsBuckets[i];
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
    free(oSymTable->ppsBuckets);

    /* Update to new buckets */
    oSymTable->ppsBuckets = ppsNewBuckets;
    oSymTable->uBucketCount = uNewBucketCount;
    oSymTable->uBucketIndex++;

    return 1;  /* Success */
}

if(oSymTable->uLength >= oSymTable->uBucketCount)
{
    SymTable_expand(oSymTable); 
}