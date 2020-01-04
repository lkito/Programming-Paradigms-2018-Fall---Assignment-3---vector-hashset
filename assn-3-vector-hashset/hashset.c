#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn){
	assert(elemSize > 0 && numBuckets > 0 && hashfn != NULL && comparefn != NULL);
	h->elements = 0;
	h->size = numBuckets;
	h->buckets = malloc(sizeof(vector) * numBuckets);
	for(int i = 0; i < numBuckets; i++){
		VectorNew(&h->buckets[i], elemSize, freefn, 0);
	}
	h->hashfn = hashfn;
	h->comparefn = comparefn;
}

void HashSetDispose(hashset *h){
	for(int i = 0; i < h->size; i++){
		VectorDispose(&h->buckets[i]);
	}
	free(h->buckets);
}

int HashSetCount(const hashset *h){
	return h->elements;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData){
	assert(mapfn != NULL);
	for(int i = 0; i < h->size; i++){
		if(VectorLength(&h->buckets[i]) != 0){
			VectorMap(&h->buckets[i], mapfn, auxData);
		}
	}
}

void HashSetEnter(hashset *h, const void *elemAddr){
	assert(elemAddr != NULL);
	int index = h->hashfn(elemAddr, h->size);
	assert(index >= 0 && index < h->size);
	int result = VectorSearch(&h->buckets[index], elemAddr, h->comparefn, 0, false);
	if(result != -1){
		VectorReplace(&h->buckets[index], elemAddr, result);
	} else {
		h->elements++;
		VectorAppend(&h->buckets[index], elemAddr);
	}
}

void *HashSetLookup(const hashset *h, const void *elemAddr){
	assert(elemAddr != NULL);
	int index = h->hashfn(elemAddr, h->size);
	assert(index >= 0 && index < h->size);
	if(VectorLength(&h->buckets[index]) == 0) return NULL;
	for(int i = 0; i < VectorLength(&h->buckets[index]); i++){
		int result = VectorSearch(&h->buckets[index], elemAddr, h->comparefn, 0, false);
		if(result != -1) return VectorNth(&h->buckets[index], result);
	}
	return NULL;
}
