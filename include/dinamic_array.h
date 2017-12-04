//
// Created by Admin on 12/4/17.
//

#ifndef GRDB_DINAMIC_ARRAY_H
#define GRDB_DINAMIC_ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include "vertexid.h"


typedef struct {
    int *array;
    size_t used;
    size_t size;
} Array;

void initArray(Array *a, size_t initialSize);

void insertArray(Array *a, int element);

void freeArray(Array *a);


#endif //GRDB_DINAMIC_ARRAY_H
