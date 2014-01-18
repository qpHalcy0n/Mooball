#include "quickindex_t.h"


#define INIT_ALLOC      1000

static int indexBinaryTraverse(
                   void* value,                             // pointer to the reference element
                   void* data,                              // pointer to the indexed data
                   int* index,                              // pointer to the indices
                   int stride,                              // data stride
                   int count,                               // number of items in the index
                   int* result,                             // buffer for the result of the last compare
                   int (*compare)(const void*, const void*) // pointer to the compare function
                   );

int indexAddElement(index_t* index, int value, int position, char append);
int indexAppend(index_t* index, int value, int position);
int indexAppendBack(index_t* index, int value);
int indexCheckAlloc(index_t* index);
int indexInsert(index_t* index, int value, int position);
int indexRemovePosition(index_t* index, int position);
int indexRemoveSortedElement(index_t* index, void* element);
int indexSortedInsert(index_t* ind, void* element);
int indexSortedInsertDup(index_t* ind, int value);
int indexFind(index_t* index, void* element, int* result);


void  indexArray(index_t* index, char* arrayObj, int size, int count, sortFunc_t sortFunc)
{
   int   i;

   memset(index, 0, sizeof(index_t));
   index->data = arrayObj;
   index->dataSize = size;
   index->sortFunc = sortFunc;

   for (i = 0; i < count; i++)
   {
      indexSortedInsert(index, i);
   }
}

void  indexArrayCB(index_t* index, char* arrayObj, int size, int count, sortFunc_t sortFunc, sortFunc_t callback)
{
   int   i;
   int   position;
   int   compValue;
   int   result;

   memset(index, 0, sizeof(index_t));
   index->data = arrayObj;
   index->dataSize = size;
   index->sortFunc = sortFunc;

   for (i = 0; i < count; i++)
   {

      position = indexBinaryTraverse(
                  (char*)index->data + (i * index->dataSize), index->data, index->indices, index->dataSize, index->count, &compValue, index->sortFunc);

      if (compValue == 0)
      {
         callback((char*)index->data + (index->indices[position] * index->dataSize), (char*)index->data + (i * index->dataSize));
      }
      else if (compValue < 0) // we are inserting before this index
      {
         result = indexInsert(index, i, position);
      }
      else // we are appending after this index
      {
         result = indexAppend(index, i, position);
      }
   }
}

void  indexSortArray(index_t* index, char* arrayObj, int size, int count, sortFunc_t sortFunc)
{
   int   i;

   memset(index, 0, sizeof(index_t));
   index->data = arrayObj;
   index->dataSize = size;
   index->sortFunc = sortFunc;

   for (i = 0; i < count; i++)
   {
      indexSortedInsertDup(index, i);
   }
}

int indexAddElement(index_t* index, int value, int position, char append)
{
   int result;

   result = indexCheckAlloc(index);
   if (result == INDEX_MEMORY_ERROR)
   {
      return result;
   }

   if (append == TRUE)
   {
      result = indexAppend(index, value, position);
   }
   else
   {
      result = indexInsert(index, value, position);
   }

   return result;
}

int indexAppend(index_t* index, int value, int position)
{
   int result;

   if ((position < 0) || ((position >= index->count) && (index->count != 0)))
   {
      return INDEX_OUT_OF_RANGE;
   }

   result = indexCheckAlloc(index);
   if (result == INDEX_MEMORY_ERROR)
      return result;


   if ((index->count != 0) && (position < (index->count - 1)))
      memmove(&index->indices[position + 2], &index->indices[position+ 1], (index->count - position - 1) * sizeof(int));


   index->indices[position + 1] = value;

   index->count++;

   return INDEX_OK;
}

int indexAppendBack(index_t* index, int value)
{
   int result;

   result = indexCheckAlloc(index);
   if (result == INDEX_MEMORY_ERROR)
      return result;

   index->indices[index->count] = value;

   index->count++;

   return INDEX_OK;
}

int indexCheckAlloc(index_t* index)
{
   int*    buffer;
   int    allocSize = 0;

   if ((index->allocCount == 0) && (index->count == 0))
   {
      index->allocCount = INIT_ALLOC;
      index->indices = (int*)malloc(index->allocCount * sizeof(int));
      if (index->indices == NULL)
      {
         return INDEX_MEMORY_ERROR;
      }
      memset(index->indices, 0, index->allocCount * sizeof(int));
   }

   if (index->count >= index->allocCount)
   {
      allocSize = (index->count << 1);

      buffer = (int*)malloc(allocSize  * sizeof(int));
      if (buffer == NULL)
      {
         return INDEX_MEMORY_ERROR;
      }

      memset(buffer, 0, allocSize * sizeof(int));

      if ((index->allocCount != 0) && (index->indices != NULL))
         memcpy(buffer, index->indices, index->allocCount * sizeof(int));


      if (index->indices != NULL)
         free(index->indices);


      index->indices = buffer;

      index->allocCount = allocSize;

      return INDEX_REALLOCATE;
   }

   return INDEX_OK;
}

void indexFree(index_t* index)
{
   if (index->indices != NULL)
   {
      free(index->indices);
   }

   memset(index, 0, sizeof(index_t));
}

int indexInsert(index_t* index, int value, int position)
{
   int result;

   if ((position < 0) || ((position >= index->count) && (index->count != 0)))
   {
      return INDEX_OUT_OF_RANGE;
   }

   result = indexCheckAlloc(index);
   if (result == INDEX_MEMORY_ERROR)
   {
      return result;
   }


   if (index->count != 0)
   {
      memmove(&index->indices[position + 1], &index->indices[position], (index->count - position) * sizeof(int));
   }


   index->indices[position] = value;

   index->count++;

   return INDEX_OK;
}

int indexRemovePosition(index_t* index, int position)
{
   int result = INDEX_NOT_FOUND;

   if ((position < 0) || (position >= index->count))
   {
      return INDEX_OUT_OF_RANGE;
   }


   if (position < (index->count - 1))
   {
      memmove(&index->indices[position], &index->indices[position + 1], (index->count - position - 1) * sizeof(int));
   }

   index->count--;

   return INDEX_OK;
}

int indexRemoveSortedElement(index_t* index, void* element)
{
   int position;
   int value;
   int result = INDEX_NOT_FOUND;


   position = indexBinaryTraverse(
               element, index->data, index->indices, index->dataSize, index->count, &value, index->sortFunc);

   if (value == 0)
   {
      result = indexRemovePosition(index, position);
   }

   return result;
}

int indexSortedInsert(index_t* ind, int value)
{
   int position;
   int compValue;
   int result;


   position = indexBinaryTraverse(
               (char*)ind->data + (value * ind->dataSize), ind->data, ind->indices, ind->dataSize, ind->count, &compValue, ind->sortFunc);

   if (compValue == 0)
   {
      result = INDEX_OK;
   }
   else if (compValue < 0) // we are inserting before this index
   {
      result = indexInsert(ind, value, position);
   }
   else // we are appending after this index
   {
      result = indexAppend(ind, value, position);
   }

   return result;
}

int indexSortedInsertDup(index_t* ind, int value)
{
   int position;
   int compValue;
   int result;


   position = indexBinaryTraverse(
               (char*)ind->data + (value * ind->dataSize), ind->data, ind->indices, ind->dataSize, ind->count, &compValue, ind->sortFunc);

   if (compValue <= 0) // we are inserting before this index
   {
      result = indexInsert(ind, value, position);
   }
   else // we are appending after this index
   {
      result = indexAppend(ind, value, position);
   }

   return result;
}

int indexFind(index_t* index, void* element, int* result)
{
   int position;


   position = indexBinaryTraverse(
               element, index->data, index->indices, index->dataSize, index->count, result, index->sortFunc);

   return position;
}

int indexBinaryTraverse(
                   void* value,                             // pointer to the reference element
                   void* data,                              // pointer to the indexed data
                   int* indices,                            // pointer index
                   int stride,                              // data stride
                   int count,                               // number of items in the array
                   int* result,                             // buffer for the result of the last compare
                   int (*compare)(const void*, const void*) // pointer to the compare function
                   )
{
   int high;
   int low;
   int mid;
   int nextMid;

   high = count;
   low = 0;
   mid = low + ((high - low) >> 1);
   *result = -1;

   while (low != high)
   {
      *result = compare(value, (void*)((unsigned int*)data + indices[mid] * stride));
      if (*result == 0)
      {
         break;
      }
      else if (*result < 0)
      {
         high = mid;
      }
      else
      {
         low = mid;
      }

      nextMid = low + ((high - low) >> 1);
      if (mid == nextMid)
      {
         break;
      }

      mid = nextMid;
   }

   return mid;
}
