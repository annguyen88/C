#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*
 * Number of bits in the address space
 */
#define NBITS 24

/*
 * Global variables and counters
 */

int blockSize = 32;
int numberOfBlocks = 0;
int numberOfSets = 64;
int associativity = 1;
int writePolicy = 0; // 0 for write-through and 1 for write-back

int tagBits = 0;
int indexBits = 0;
int offsetBits = 0;
int cacheSize = 0;

int numberOfRefs = 0;
int numberOfReads = 0;
int numberOfWrites = 0;
int numberOfHits = 0;
int numberOfMisses = 0;
int mainMemReads = 0;
int mainMemWrites = 0;

/*
 * Queue Structures
 */

/*
 * Queue for holding memory references
 */
typedef struct refq RefQ;

struct refq {
    char type; // 0 for Read and 1 for Write
    int value;
    RefQ *next;
};

/*
 * Queue for holding blocks
 */
typedef struct blockq BlockQ;

struct blockq {
    int tag;
    int index;
    int valid; // 0 for invalid and 1 for valid
    int dirty; // 0 for not dirty and 1 for dirty
    int timeStamp; // Larger value specifies a more recent use
    BlockQ *next;
};

/*
 * Queue for holding sets
 */
typedef struct setq SetQ;

struct setq {
    BlockQ *blocks;
    SetQ *next;
};

/*
 * Queue for holding output trace
 */
typedef struct traceq TraceQ;

struct traceq {
    int hexAddress;
    char refType;
    int binaryAddress;
    int tag;
    int index;
    int offset;
    int readWay;
    int updateWay;
    int reads;
    int writes;
    TraceQ *next;
};

SetQ *cache; // Holds the cache

/*
 * Queue Operations
 */

RefQ *KillRefInst(RefQ * dead) {
    RefQ *next;
    if (dead) {
        next = dead->next;
        free(dead);
    } else next = NULL;
    return (next);
}

RefQ *NewRefInst(char type, int value) {
    RefQ *np;
    np = malloc(sizeof (RefQ));

    np->type = type;
    np->value = value;
    np->next = NULL;
    return (np);
}

BlockQ *KillBlockInst(BlockQ * dead) {
    BlockQ *next;
    if (dead) {
        next = dead->next;
        free(dead);
    } else next = NULL;
    return (next);
}

BlockQ *NewBlockInst(int tag, int index, int valid, int dirty) {
    BlockQ *np;
    np = malloc(sizeof (BlockQ));

    np->tag = tag;
    np->index = index;
    np->valid = valid;
    np->dirty = dirty;
    np->timeStamp = 0;
    return (np);
}

SetQ *KillSetInst(SetQ * dead) {
    SetQ *next;
    if (dead) {
        next = dead->next;
        free(dead);
    } else next = NULL;
    return (next);
}

SetQ *NewSetInst(BlockQ * block) {
    SetQ *np;
    np = malloc(sizeof (SetQ));

    np->blocks = block;
    np->next = NULL;
    return (np);
}

TraceQ *KillTraceInst(TraceQ *dead) {
    TraceQ *next;
    if (dead) {
        next = dead->next;
        free(dead);
    } else next = NULL;
    return (next);
}

TraceQ *NewTraceInst(int hexAddress, char refType, int binaryAddress, int tag,
        int index, int offset, int readWay, int updateWay, int reads, int writes) {
    TraceQ *np;
    np = malloc(sizeof (TraceQ));

    np->hexAddress = hexAddress;
    np->refType = refType;
    np->binaryAddress = binaryAddress;
    np->tag = tag;
    np->index = index;
    np->offset = offset;
    np->readWay = readWay;
    np->updateWay = updateWay;
    np->reads = reads;
    np->writes = writes;

    return (np);
}

/*
 * Helper Functions
 */

char *FormatBits(char *input, int tag, int index, int offset) {
    static char output[NBITS + 3];
    output[NBITS + 2] = '\0';
    output[tag] = output[index + tag + 1] = ' ';
    int i;

    for (i = 0; i < tag; i++)
        output[i] = input[i];

    for (i = tag + 1; i < tag + index + 1; i++)
        output[i] = input[i - 1];

    for (i = tag + index + 2; i < tag + index + offset + 2; i++)
        output[i] = input[i - 2];

    return output;
}

char *Int2Bits(unsigned int bits) {
    static char cbits[NBITS + 1];
    int i;

    cbits[NBITS] = '\0';
    for (i = 0; i < NBITS; i++)
        cbits[NBITS - 1 - i] = (bits == ((1 << i) | bits)) ? '1' : '0';

    return cbits;
}

unsigned int GetMask(int nset) {
    unsigned int mask, i;

    assert(nset <= 32); /* Otherwise default int won't work */

    mask = ((((long long) 1)) << nset) - 1;

    for (mask = i = 0; i < nset; i++)
        mask |= (1 << i);

    return mask;
}

int GetLowerBits(int number, int bits) {
    int mask = GetMask(bits);

    return (number & mask);
}

int Log2(int number) {
    int i = 1, count = 0;
    while (i != number) {
        i *= 2;
        count++;
    }
    return count;
}

int IsPowerOfTwo(int x) {
    while (((x % 2) == 0) && x > 1) /* While x is even and > 1 */
        x /= 2;
    return (x == 1);
}

/*
 * Cache Operations
 */

BlockQ *GetSetBlocks(int index) {
    SetQ *ip, *ibase;
    ibase = cache;
    int i = 0;

    for (i = 0, ip = ibase; i < index; ip = ip->next, i++) {
    }

    return ip->blocks;
}

BlockQ *GetBlock(int index, int way) {
    BlockQ *blockBase = GetSetBlocks(index);
    BlockQ *blockIp = blockBase;

    int i = 0;
    for (i = 0, blockIp = blockBase; i < way; blockIp = blockIp->next, i++) {
    }
    return blockIp;
}

void WriteBlock(int tag, int index, int way, int valid, int dirty, int timeStamp) {
    BlockQ *blockBase = GetSetBlocks(index);
    BlockQ *blockIp = blockBase;

    int i = 0;
    for (i = 0, blockIp = blockBase; i < way; blockIp = blockIp->next, i++) {
    }
    blockIp->tag = tag;
    blockIp->index = index;
    blockIp->valid = valid;
    blockIp->dirty = dirty;
    blockIp->timeStamp = timeStamp;
}

int GetLRUWay(int index) {
    BlockQ *blockBase = GetSetBlocks(index);
    BlockQ *blockIp = blockBase;

    int i = 0, way = 0;
    int min = 1000;
    while (blockIp) {
        if ((blockIp->timeStamp) < min) {
            min = blockIp->timeStamp;
            way = i;
        }
        i++;
        blockIp = blockIp->next;
    }

    return way;
}

int IsInCache(int tag, int index, int *way, int *dirty, int *timeStamp) {
    BlockQ* blockBase = GetSetBlocks(index);
    BlockQ *blockIp = blockBase;
    int i = 0;

    while (blockIp) {
        if ((blockIp->tag == tag) && (blockIp->index == index) && (blockIp->valid == 1)) {
            *way = i;
            *dirty = blockIp->dirty;
            *timeStamp = blockIp->timeStamp;
            return 1;
        }
        i++;
        blockIp = blockIp->next;
    }
    return 0;
}

/*
 * Main Methods
 */

void PrintSummary() {
    printf("\n");
    printf("nref=%d, nread=%d, nwrite=%d\n", numberOfRefs, numberOfReads, numberOfWrites);
    printf("   hits   =%10d, hit  rate = %1.2f\n", numberOfHits, (double) numberOfHits / numberOfRefs);
    printf("   misses =%10d, miss rate = %1.2f\n", numberOfMisses, (double) numberOfMisses / numberOfRefs);
    printf("   main memory reads=%d, main memory writes=%d\n", mainMemReads, mainMemWrites);
}

void PrintTrace(TraceQ *ibase, FILE *fpout) {
    TraceQ *ip;
    int i;

    for (i = 0, ip = ibase; ip; ip = ip->next, i++) {
        fprintf(fpout, "%6x%c %26s %8d %5d %3d %4d %4d  %4d %4d\n",
                ip->hexAddress, ip->refType, FormatBits(Int2Bits(ip->binaryAddress), tagBits, indexBits, offsetBits),
                ip->tag, ip->index, ip->offset, ip->readWay, ip->updateWay, ip->reads, ip->writes);
    }
}

TraceQ *ProcessReferences(RefQ *refBase) {
    RefQ *refIp;
    int i, j = 0;
    BlockQ *block;
    TraceQ *traceBase, *traceIp;
    traceIp = traceBase = NewTraceInst(0, 'R', 0, 0, 0, 0, 0, 0, 0, 0);

    for (i = 0, refIp = refBase; refIp; refIp = refIp->next, i++) {
        numberOfRefs++;
        int offset = GetLowerBits(refIp->value, offsetBits);
        int index = GetLowerBits(refIp->value >> offsetBits, indexBits);
        int tag = GetLowerBits(refIp->value >> (indexBits + offsetBits), tagBits);

        traceIp->hexAddress = refIp->value;
        traceIp->binaryAddress = refIp->value;
        traceIp->tag = tag;
        traceIp->index = index;
        traceIp->offset = offset;

        int dirty = 0, way = 0, timeStamp = 0;
        int inCache = IsInCache(tag, index, &way, &dirty, &timeStamp);

        if (refIp->type == 'R') {
            // Read reference
            traceIp->refType = 'R';
            numberOfReads++;
            if (inCache) {
                // If it's a read and is in the cache, it's a hit.
                // So we read from cache and update the timestamp.

                WriteBlock(tag, index, way, 1, 0, i + 1);

                traceIp->readWay = way;
                traceIp->updateWay = -2;
                traceIp->reads = 0;
                traceIp->writes = 0;

                numberOfHits++;
            } else {
                // If it's a read and is not in the cache, it's a miss.
                // So we read from main memory.
                traceIp->readWay = -1;

                int wayToWrite = GetLRUWay(index);
                traceIp->readWay = -1;
                traceIp->updateWay = wayToWrite;

                // Read from memory
                if ((writePolicy == 1)) {
                    // For write-back we may need to
                    // write to main memory on eviction.
                    BlockQ *block = GetBlock(index, wayToWrite);
                    if (block->dirty) {
                        traceIp->reads = 1;
                        mainMemReads++;
                        traceIp->writes = 1;
                        mainMemWrites++;
                    } else {
                        traceIp->reads = 1;
                        mainMemReads++;
                        traceIp->writes = 0;
                    }
                } else {
                    traceIp->reads = 1;
                    mainMemReads++;
                    traceIp->writes = 0;
                }

                WriteBlock(tag, index, wayToWrite, 1, 0, i + 1);
                numberOfMisses++;
            }
        } else {
            // Write reference
            traceIp->refType = 'W';
            numberOfWrites++;
            if (inCache) {
                // If it's a write and is in the cache, it's a hit.
                // So we do nothing but updating the timestamp.
                if (writePolicy == 0)
                    WriteBlock(tag, index, way, 1, dirty, i + 1);
                else
                    WriteBlock(tag, index, way, 1, 1, i + 1);

                traceIp->reads = 0;
                if (writePolicy == 0) {
                    traceIp->readWay = way;
                    traceIp->updateWay = -2;
                    traceIp->writes = 1;
                    mainMemWrites++;
                } else {
                    traceIp->readWay = way;
                    traceIp->writes = 0;
                    traceIp->updateWay = way;
                }

                numberOfHits++;
            } else {
                if (writePolicy == 0) {
                    // If it's a write and is not in the cache, it's a miss.
                    // So for write-through we just write to main memory.
                    traceIp->readWay = -1;
                    traceIp->updateWay = -1;
                    traceIp->reads = 0;
                    traceIp->writes = 1;
                    mainMemWrites++;

                    numberOfMisses++;
                } else {
                    // If it's a write and is not in the cache, it's a miss.
                    // So for write-back we read from the main memory.

                    int wayToWrite = GetLRUWay(index);

                    BlockQ *block = GetBlock(index, wayToWrite);
                    if (block->dirty) {
                        traceIp->readWay = -1;
                        traceIp->updateWay = wayToWrite;
                        traceIp->reads = 1;
                        mainMemReads++;
                        traceIp->writes = 1;
                        mainMemWrites++;
                    } else {
                        traceIp->readWay = -1;
                        traceIp->updateWay = wayToWrite;
                        traceIp->reads = 1;
                        mainMemReads++;
                        traceIp->writes = 0;
                    }

                    WriteBlock(tag, index, wayToWrite, 1, 1, i + 1);

                    numberOfMisses++;
                }
            }
        }
        if (refIp->next != NULL)
            traceIp->next = NewTraceInst(0, 'R', 0, 0, 0, 0, 0, 0, 0, 0);
        traceIp = traceIp->next;
    }

    KillTraceInst(traceIp);
    return traceBase;
}

void PrintHeader() {
    if (IsPowerOfTwo(cacheSize) && (Log2(cacheSize) >= 10))
        printf("%dKB %d-way associative cache:\n", (cacheSize / 1024), associativity);
    else
        printf("%d byte %d-way associative cache:\n", cacheSize, associativity);

    printf("   Block size = %d bytes\n", blockSize);
    printf("   Number of [sets,blocks] = [%d,%d]\n", numberOfSets, numberOfBlocks);
    printf("   Extra space for tag storage = %d bytes ( %2.2f%%)\n",
            ((tagBits * numberOfBlocks) / 8),
            (((double) ((tagBits * numberOfBlocks) / 8) / (blockSize * numberOfBlocks)) * 100));
    printf("   Bits for [tag,index,offset] = [%d, %d, %d] = %d\n",
            tagBits, indexBits, offsetBits, (tagBits + indexBits + offsetBits));
    printf("   Write policy = %s\n\n", ((writePolicy == 0) ? "Write-through" : "Write-back"));
    printf("Hex     Binary Address                       Set  Blk               Memory\n");
    printf("Address (tag/index/offset)              Tag Index off  Way UWay  Read Writ\n");
    printf("======= ========================== ======== ===== === ==== ====  ==== ====\n");
}

void PrintCache(SetQ *setBase, FILE * fpout) {
    SetQ *setIp;
    int i;

    fprintf(fpout, "Cache:\n");
    for (i = 0, setIp = setBase; setIp; setIp = setIp->next, i++) {
        BlockQ *blockBase = GetSetBlocks(i);
        BlockQ *blockIp = blockBase;
        while (blockIp) {
            fprintf(fpout, "%3d - Tag: %8d  Index: %8d  Valid: %1d Dirty: %1d timeStamp: %2d\n",
                    i, blockIp->tag, blockIp->index, blockIp->valid, blockIp->dirty, blockIp->timeStamp);
            blockIp = blockIp->next;
        }
        fprintf(fpout, "-----------------------------------------------------------------------\n");
    }
    fprintf(fpout, "\n");
}

SetQ *BuildCache() {
    SetQ *setBase, *setIp;
    BlockQ *blockBase, *blockIp;

    setIp = setBase = NewSetInst(NULL);

    int i = 0, j = 0;

    for (i = 0; i < numberOfSets; i++) {
        BlockQ *blockBase, *blockIp;
        blockIp = blockBase = NewBlockInst(0, 0, 0, 0);
        for (j = 0; j < associativity; j++) {
            blockIp->next = NewBlockInst(0, 0, 0, 0);
            blockIp = blockIp->next;
        }

        setIp->next = NewSetInst(KillBlockInst(blockBase));
        setIp = setIp->next;
    }
    return (KillSetInst(setBase));
}

void ComputeSizes() {
    numberOfBlocks = numberOfSets * associativity;
    indexBits = Log2(numberOfSets);
    offsetBits = Log2(blockSize);
    tagBits = NBITS - (indexBits + offsetBits);
    cacheSize = numberOfSets * associativity * blockSize;
    if (associativity == 0) {
        numberOfBlocks = numberOfSets;
        associativity = numberOfBlocks;
        numberOfSets = 1;
        indexBits = 0;
        tagBits = NBITS - (indexBits + offsetBits);
        cacheSize = numberOfSets * associativity * blockSize;
    }
}

void PrintReferences(RefQ *ibase, FILE *fpout) {
    RefQ *ip;
    int i;

    fprintf(fpout, "References:\n");
    for (i = 1, ip = ibase; ip; ip = ip->next, i++) {

        fprintf(fpout, "%c:%x\n", ip->type, ip->value);

    }
    fprintf(fpout, "\n");
}

RefQ *ReadReferences(FILE * fpin) {
    char ln[256];
    char type;
    int value;
    RefQ *ibase, *ip;

    ip = ibase = NewRefInst('R', 0);

    while (fgets(ln, 256, fpin)) {
        sscanf(ln, "%c:%x", &type, &value);

        ip->next = NewRefInst(type, value);
        ip = ip->next;
    }
    return (KillRefInst(ibase));
}

int GetFlags(int nargs, char **args) {
    int i = 0;
    for (i = 1; i < nargs - 1; i++) {
        if (strcmp(args[i], "-b") == 0) {
            if (IsPowerOfTwo(atoi(args[i + 1])))
                blockSize = atoi(args[i + 1]);
            else {
                printf("ERROR: block size (%d) must be a power of two!\n\n", atoi(args[i + 1]));
                return 1;
            }
        } else if (strcmp(args[i], "-s") == 0) {
            if (IsPowerOfTwo(atoi(args[i + 1])))
                numberOfSets = atoi(args[i + 1]);
            else {
                printf("ERROR: number of sets (%d) must be a power of two!\n\n", atoi(args[i + 1]));
                return 1;
            }
        } else if (strcmp(args[i], "-a") == 0) {
            associativity = atoi(args[i + 1]);
        } else if (strcmp(args[i], "-W") == 0) {
            if (strcmp(args[i + 1], "t") == 0)
                writePolicy = 0;
            else if (strcmp(args[i + 1], "b") == 0)
                writePolicy = 1;
        }
    }
    return 0;
}

int main(int nargs, char **args) {
    int error = GetFlags(nargs, args);

    RefQ* references = ReadReferences(stdin);

    if (!error) {
        ComputeSizes();
        cache = BuildCache();
        PrintHeader();
        TraceQ *trace = ProcessReferences(references);
        PrintTrace(trace, stdout);
        PrintSummary();
    }

    return 0;
}

