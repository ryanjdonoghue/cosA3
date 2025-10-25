/*--------------------------------------------------------------------*/
/* symtable.h                                                         */
/* Author: Ryan Donoghue                                              */
/*--------------------------------------------------------------------*/
#ifndef SYMTABLE_H
#define SYMTABLE_H

/* A SymTable_T is an unordered collection of bindings. A binding 
consists of a key and a value. A key is a string that uniquely 
identifies its binding; a value is data that is somehow pertinent 
to its key. */

typedef struct SymTable *SymTable_T;

/*--------------------------------------------------------------------*/

/* Return a new SymTable_T object, or NULL if insuficient memory is
available. */

SymTable_T SymTable_new(void);

/*--------------------------------------------------------------------*/

/* Free oSymTable. */
void SymTable_free(SymTable_T oSymTable);

/*--------------------------------------------------------------------*/

/* Get the length of oSymTable. */
size_t SymTable_getLength(SymTable_T oSymTable);

/*--------------------------------------------------------------------*/

/* Put binding of pcKey and pvValue into oSymTable. Return 1 (True) if
successful, or 0 (FALSE) if insufficient memory is available. */

int SymTable_put(SymTable_T oSymTable, 
    const char *pcKey, const void *pvValue); 

/*--------------------------------------------------------------------*/

/* if oSymTable contains a binding with key pcKey, then replace the 
binding's value with pvValue and return the old value. Otherwise return 
NULL. */

void *SymTable_replace(SymTable_T oSymTable,
    const char *pcKey, const void *pvValue); 

/*--------------------------------------------------------------------*/

/* Return 1 (TRUE) if oSymTable contains pcKey, or 0 (FALSE) 
otherwise. */

int SymTable_contains(SymTable_T oSymTable, const char *pcKey); 

/*--------------------------------------------------------------------*/


/* Return the value of the binding within oSymTable whose key is 
pcKey, or NULL if no such binding exists. */

void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/*--------------------------------------------------------------------*/

/* If oSymTable contains a binding with key pcKey, remove that binding 
from oSymTable and return the binding's value. Otherwise return NULL. */

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey); 

/*--------------------------------------------------------------------*/

/*  */

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra);

#endif
