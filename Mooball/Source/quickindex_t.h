#ifndef     __QUICKINDEX_T_H_
#define     __QUICKINDEX_T_H_



#include <stdlib.h>
#include <memory.h>
#include <string.h>


#define INDEX_OK                0
#define INDEX_REALLOCATE        -1
#define INDEX_MEMORY_ERROR      -2
#define INDEX_OUT_OF_RANGE      -3
#define INDEX_NOT_FOUND         -4

#ifndef TRUE
   #define TRUE                  1
#endif
#ifndef FALSE
   #define FALSE                 0
#endif


typedef int (*sortFunc_t)(const void*, const void*);

typedef struct index_s
{
   int         count;
   int         allocCount;
   int         *indices;
   char        *data;
   int         dataSize;
   sortFunc_t  sortFunc;
} index_t;


int   indexFind(index_t* v, void* element, int* result);
void  indexFree(index_t* v);
int   indexSortedInsert(index_t* v, int value);
void  indexArray(index_t* index, char* arrayObj, int size, int count, sortFunc_t sortFunc);
void  indexArrayCB(index_t* index, char* arrayObj, int size, int count, sortFunc_t sortFunc, sortFunc_t callbackFunc);
void  indexSortArray(index_t* index, char* arrayObj, int size, int count, sortFunc_t sortFunc);




#endif
