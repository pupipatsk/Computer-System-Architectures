/**
 * CacheSim.h
 * This program is designed for class exercise only.
 * It is provided as is. There is no warranty or support of any kind.
 *
 * Krerk Piromsopa, Ph.D.
 * Department of Computer Engineering
 * Chulalongkorn University
 * Bangkok, Thailand.
**/

#ifndef CACHESIM_H_INCLUDED
#define CACHESIM_H_INCLUDED

#include <math.h>
#include <stdbool.h>

#define MAX_INDEX_SIZE 1024 // Max index size (adjustable if needed)

typedef struct CacheLine {
    bool valid;
    bool dirty;
    unsigned long tag;
} CacheLine;

#endif // CACHESIM_H_INCLUDED
