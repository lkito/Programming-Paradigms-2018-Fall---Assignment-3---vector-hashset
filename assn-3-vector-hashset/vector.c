#include "vector.h"
#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//Starting size of a new vector, which will be used if user specifies it incorrectly.
const int START_SIZE = 10;

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation){
    v->logLen = 0;
    if(initialAllocation <= 0){
        v->allocLen = START_SIZE;
    } else v->allocLen = initialAllocation;
    v->freefn = freeFn;
    v->elemSize = elemSize;
    v->data = malloc(elemSize * v->allocLen);
}

void VectorDispose(vector *v){
    if(v->freefn != NULL){
        for(int i = 0; i < v->logLen; i++){
            void *cur = (char*)v->data + i * v->elemSize;
            v->freefn(cur);
        }
    }
    free(v->data);
}

int VectorLength(const vector *v){
    assert(v != NULL);
    return v->logLen;
}

void *VectorNth(const vector *v, int position){
    assert(position >= 0 && position < v->logLen);
    return ((char*)v->data + position * v->elemSize);
}

void VectorReplace(vector *v, const void *elemAddr, int position){
    assert(position >= 0 && position < v->logLen);
    void *cur = (char*)v->data + position * v->elemSize;
    if(v->freefn != NULL) v->freefn(cur);
    memcpy(cur, elemAddr, v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position){
    if(position == v->logLen){
        VectorAppend(v, elemAddr);
        return;
    }
    assert(position >= 0 && position < v->logLen);
    if(v->logLen == v->allocLen) grow(v);
    memmove((char*)v->data + (position + 1) * v->elemSize, 
        (char*)v->data + position * v->elemSize, v->elemSize * (v->logLen - position));
    memcpy((char*)v->data + position * v->elemSize, elemAddr, v->elemSize);
    v->logLen++;
}

void VectorAppend(vector *v, const void *elemAddr){
    if(v->logLen == v->allocLen) grow(v);
    memcpy((char*)v->data + v->logLen * v->elemSize, elemAddr, v->elemSize);
    v->logLen++;
}

void VectorDelete(vector *v, int position){
    assert(position >= 0 && position < v->logLen);
    if(v->freefn != NULL) v->freefn((char*)v->data + position * v->elemSize);
    memmove((char*)v->data + position * v->elemSize, 
        (char*)v->data + (position + 1) * v->elemSize, v->elemSize * (v->logLen - position - 1));
    v->logLen--;
}

void VectorSort(vector *v, VectorCompareFunction compare){
    assert(compare != NULL && v != NULL);
    qsort(v->data, v->logLen, v->elemSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData){
    assert(mapFn != NULL);
    for(int i = 0; i < v->logLen; i++){
        mapFn((char*)v->data + i * v->elemSize, auxData);
    }
}

void grow(vector *v){
    v->allocLen *= 2;
    v->data = realloc(v->data, v->allocLen * v->elemSize);
}

static const int kNotFound = -1;

int VectorSearch(const vector *v, const void *key, 
        VectorCompareFunction searchFn, int startIndex, bool isSorted){
    assert(startIndex >= 0 && startIndex <= v->logLen);
    char* resultLoc;
    size_t nMemb = v->logLen - startIndex;
    if(isSorted){
        resultLoc = (char*)bsearch(key, (char*)v->data + startIndex * v->elemSize,
            nMemb, v->elemSize, searchFn);
    } else {
        resultLoc = (char*)lfind(key, (char*)v->data + startIndex * v->elemSize, 
            &nMemb, v->elemSize, searchFn);
    }
    if(resultLoc == NULL){
        return kNotFound;
    } else return ((char*)resultLoc - (char*)v->data)/v->elemSize;
}
