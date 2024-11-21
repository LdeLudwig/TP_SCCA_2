#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <unistd.h> // Necessário para sleep e usleep

#define MAX_ALLOCATIONS 1000
#define HEAP_SIZE 10000

#pragma pack(push,1)
struct mem_block{
    int is_free;
    size_t size;
    void* mem_ptr;
    struct mem_block* next;
    struct mem_block * prev;
};
#pragma pack(pop)

struct allocation_t {
    void *ptr;
    unsigned int size;
} allocations[MAX_ALLOCATIONS];

void *heap_start = NULL;
struct mem_block *block_list = NULL;
unsigned int heap_used = 0;

void *smalloc(unsigned int size) {
    if (heap_start == NULL) {
        heap_start = sbrk(HEAP_SIZE);
        if (heap_start == (void *)-1) {
            perror("Erro ao inicializar o heap com sbrk");
            exit(1);
        }
        // Cria primeiro bloco da lista encadeada
        block_list = (struct mem_bloc *)heap_start;
        block_list->is_free = 1;
        block_list->size = HEAP_SIZE - sizeof(struct mem_block);
        block_list->prev = NULL;
        block_list->next = NULL;
        heap_used = 0;
    }
    struct mem_block *current = block_list;

    while (current != NULL){
        if(current->is_free == 1 && current->size >= size){
            if(current->size > size + sizeof(struct mem_block)){
                struct mem_block *new_block = (struct mem_block *)((char *)current + sizeof(struct mem_block) + size);
                new_block->is_free = 1;
                new_block->size = current->size -size - sizeof(struct mem_block);
                new_block->next = current->next;
                new_block->prev = current;

                if(current->next != NULL){
                    current->next->prev = new_block;
                }
                current->next = new_block;
                current->size = size;
            }
            current->is_free = 0;
            heap_used += size;
            return (char *)current + sizeof(struct mem_block);
        }
        current = current->next;
    }
    
    printf("Out of memory in smalloc\n");
    return NULL;
}

void sfree(void *ptr, unsigned int size) {
    /* ---------------------------------------------------------------------------
        
        Verificação se há blocos adjacente e lógica para liberação dos blocos aqui

     -----------------------------------------------------------------------------*/
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

