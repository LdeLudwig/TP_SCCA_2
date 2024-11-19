#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <unistd.h> // Necessário para sleep e usleep

#define MAX_ALLOCATIONS 1000
#define HEAP_SIZE 10000

struct allocation_t {
    void *ptr;
    unsigned int size;
} allocations[MAX_ALLOCATIONS];

void *heap_start = NULL;
unsigned int heap_used = 0;

void *smalloc(unsigned int size) {
    if (heap_start == NULL) {
        heap_start = sbrk(HEAP_SIZE);
        if (heap_start == (void *)-1) {
            perror("Erro ao inicializar o heap com sbrk");
            exit(1);
        }
        heap_used = 0;
    }

    if (heap_used + size > HEAP_SIZE) {
        printf("Out of memory in smalloc\n");
        return NULL;
    }

    void *block = heap_start + heap_used;
    heap_used += size;
    return block;
}

void sfree(void *ptr, unsigned int size) {
    heap_used -= size; // Apenas ajusta o uso
}

void printHeapStatus() {
    printf("Heap status:\n");
    printf("  Used: %u bytes\n", heap_used);
    printf("  Free: %u bytes\n", HEAP_SIZE - heap_used);
}

int main(int argc, char *argv[]) {
    unsigned int currentMemory = 0;
    int allocCount = 0;
    unsigned int maxMemory;
    unsigned int size;

    if (argc < 2) {
        printf("Usage: %s <max memory in bytes>\n", argv[0]);
        return 1;
    }

    maxMemory = atoi(argv[1]);

    srand(time(NULL));

    while (1) {
        if (rand() % 2 == 0 && allocCount < MAX_ALLOCATIONS) {
            size = rand() % 100;
            if (currentMemory + size > maxMemory) {
                printf("Out of memory\n");
                continue;
            }
            allocations[allocCount].ptr = smalloc(size);
            if (allocations[allocCount].ptr == NULL) {
                continue;
            }
            allocations[allocCount].size = size;
            currentMemory += size;
            allocCount++;
            printf("Allocated %u bytes, total %u, position %i\n", size, currentMemory, allocCount);
            printHeapStatus();
        } else {
            if (allocCount > 0) {
                allocCount--;
                size = allocations[allocCount].size;
                currentMemory -= size;
                sfree(allocations[allocCount].ptr, size);
                printf("Freed %u bytes, total %u, position %i\n", size, currentMemory, allocCount);
                printHeapStatus();
            }
        }
        usleep(5000000);  // Pausa de 500 milissegundos
    }

    return 0;
}

