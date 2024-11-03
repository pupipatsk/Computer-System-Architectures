/**
 * CacheSim.c
 * This program is designed for class exercise only.
 * It is provided as is. There is no warranty or support of any kind.
 *
 * Krerk Piromsopa, Ph.D.
 * Department of Computer Engineering
 * Chulalongkorn University
 * Bangkok, Thailand.
 *
 * History
 * 2013 - Initial design
 * 2015 - Refactor/Clean code
 * 2024 - Modified for assignment testing
**/
#include <stdio.h>
#include <stdlib.h>
#include "CacheSim.h"

long MISS;
long HIT;
struct CacheLine *cache = NULL;  // Dynamically allocated array
int BLOCK_SIZE;
int CACHE_SIZE;
int INDEX_SIZE;
int OFFSETLEN;
int INDEXLEN;

void init() {
    MISS = 0;
    HIT = 0;
    for (int i = 0; i < INDEX_SIZE; i++) {
        cache[i].valid = 0;
        cache[i].tag = 0;
        cache[i].dirty = 0;
    }
}

void configureCache(int cacheSizeKB, int blockSizeBytes) {
    BLOCK_SIZE = blockSizeBytes;
    CACHE_SIZE = cacheSizeKB * 1024;
    INDEX_SIZE = CACHE_SIZE / BLOCK_SIZE;
    OFFSETLEN = log2(BLOCK_SIZE);
    INDEXLEN = log2(INDEX_SIZE);

    // Allocate memory for cache based on INDEX_SIZE
    cache = (struct CacheLine *)malloc(INDEX_SIZE * sizeof(struct CacheLine));
    if (!cache) {
        fprintf(stderr, "Memory allocation failed for cache.\n");
        exit(-1);
    }
}

int calAddr(unsigned long addr, unsigned long *tag, unsigned long *idx, unsigned long *offset) {
    unsigned long tmp;
    *tag = addr >> (OFFSETLEN + INDEXLEN);
    *idx = (addr >> OFFSETLEN) & ((1 << INDEXLEN) - 1);
    *offset = addr & ((1 << OFFSETLEN) - 1);
    return 0;
}

int access(unsigned long addr) {
    unsigned long offset, tag, idx;
    calAddr(addr, &tag, &idx, &offset);

    if (cache[idx].tag == tag && cache[idx].valid) {
        HIT++;
    } else {
        MISS++;
        cache[idx].valid = 1;
        cache[idx].tag = tag;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    // printf("This program is designed for class exercise only.\n");
    // printf("By Krerk Piromsopa, Ph.D.\n");

    if (argc < 4) {
        fprintf(stderr, "Usage:\n\t%s address_file cache_size_kb block_size_bytes\n", argv[0]);
        exit(-1);
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        perror("Error opening address file");
        return -1;
    }

    int cacheSizeKB = atoi(argv[2]);
    int blockSizeBytes = atoi(argv[3]);
    configureCache(cacheSizeKB, blockSizeBytes);
    init();

    char buff[1025];
    unsigned long myaddr;
    while (fgets(buff, 1024, input)) {
        sscanf(buff, "0x%lx", &myaddr);
        access(myaddr);
    }

    fclose(input);
    printf("HIT:%7ld MISS:%7ld\n", HIT, MISS);
    printf("HitRate:%.4lf", (double)HIT / (HIT + MISS));
    // Free allocated memory for cache
    free(cache);
    return 0;
}
