#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 4  // Block size is fixed at 4 bytes

typedef enum { LRU, RR } ReplacementPolicy;

typedef struct CacheLine {
    int valid;
    int tag;
    int last_used;  // For LRU
} CacheLine;

typedef struct CacheSet {
    CacheLine *lines;
    int rr_index;  // For Round Robin policy
} CacheSet;

typedef struct Cache {
    CacheSet *sets;
    int num_sets;
    int associativity;
    ReplacementPolicy policy;
    int clock;  // For tracking LRU usage order
    long hits;
    long misses;
} Cache;

Cache *init_cache(int cache_size_kb, int associativity, ReplacementPolicy policy) {
    Cache *cache = malloc(sizeof(Cache));
    int num_blocks = (cache_size_kb * 1024) / BLOCK_SIZE;
    cache->num_sets = num_blocks / associativity;
    cache->associativity = associativity;
    cache->policy = policy;
    cache->clock = 0;
    cache->hits = 0;
    cache->misses = 0;

    cache->sets = malloc(cache->num_sets * sizeof(CacheSet));
    for (int i = 0; i < cache->num_sets; i++) {
        cache->sets[i].lines = malloc(associativity * sizeof(CacheLine));
        cache->sets[i].rr_index = 0;
        for (int j = 0; j < associativity; j++) {
            cache->sets[i].lines[j].valid = 0;
            cache->sets[i].lines[j].tag = -1;
            cache->sets[i].lines[j].last_used = 0;
        }
    }
    return cache;
}

void free_cache(Cache *cache) {
    for (int i = 0; i < cache->num_sets; i++) {
        free(cache->sets[i].lines);
    }
    free(cache->sets);
    free(cache);
}

int get_set_index(int address, int num_sets) {
    return (address / BLOCK_SIZE) % num_sets;
}

int get_tag(int address, int num_sets) {
    return address / (num_sets * BLOCK_SIZE);
}

void access_cache(Cache *cache, int address) {
    int set_index = get_set_index(address, cache->num_sets);
    int tag = get_tag(address, cache->num_sets);
    CacheSet *set = &cache->sets[set_index];

    // Check if there's a hit
    for (int i = 0; i < cache->associativity; i++) {
        if (set->lines[i].valid && set->lines[i].tag == tag) {
            cache->hits++;
            set->lines[i].last_used = cache->clock++;
            return;
        }
    }

    // Miss, need to replace a line
    cache->misses++;

    // Find the line to replace based on the replacement policy
    int replace_index;
    if (cache->policy == LRU) {
        // LRU Replacement Policy
        int lru_index = 0;
        int oldest_use = set->lines[0].last_used;
        for (int i = 1; i < cache->associativity; i++) {
            if (set->lines[i].last_used < oldest_use) {
                oldest_use = set->lines[i].last_used;
                lru_index = i;
            }
        }
        replace_index = lru_index;
    } else {
        // RR Replacement Policy
        replace_index = set->rr_index;
        set->rr_index = (set->rr_index + 1) % cache->associativity;
    }

    // Replace the cache line
    set->lines[replace_index].valid = 1;
    set->lines[replace_index].tag = tag;
    set->lines[replace_index].last_used = cache->clock++;
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <trace_file> <cache_size_kb> <associativity> <replacement_policy>\n", argv[0]);
        return 1;
    }

    FILE *trace_file = fopen(argv[1], "r");
    if (!trace_file) {
        perror("Error opening trace file");
        return 1;
    }

    int cache_size_kb = atoi(argv[2]);
    int associativity = atoi(argv[3]);
    ReplacementPolicy policy = (strcmp(argv[4], "LRU") == 0) ? LRU : RR;

    Cache *cache = init_cache(cache_size_kb, associativity, policy);

    char line[64];
    while (fgets(line, sizeof(line), trace_file)) {
        int address;
        sscanf(line, "0x%x", &address);
        access_cache(cache, address);
    }

    fclose(trace_file);

    printf("Cache Size: %d KB, Associativity: %d-way, Policy: %s\n", cache_size_kb, associativity, (policy == LRU) ? "LRU" : "RR");
    printf("Hits: %ld, Misses: %ld, Hit Rate: %.2f%%\n", cache->hits, cache->misses, (100.0 * cache->hits) / (cache->hits + cache->misses));

    free_cache(cache);
    return 0;
}
