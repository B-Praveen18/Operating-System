[24bcs048@mepcolinux ex7]$cat pgm.c
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX 50

/* Function to display allocation results */
void displayResults(int partitions[], int num_partitions, int processes[], int num_processes,
                    int alloc[], const char *title) {
    int i;
    int internal_frag = 0, external_frag = 0;
    int part_used[MAX] = {0};

    /* Mark used partitions and calculate internal fragmentation */
    for(i = 0; i < num_processes; i++) {
        if(alloc[i] != -1) {
            part_used[alloc[i]] = 1;
            internal_frag += (partitions[alloc[i]] - processes[i]);
        }
    }

    /* Calculate external fragmentation = total free partition space */
    for(i = 0; i < num_partitions; i++) {
        if(!part_used[i]) {
            external_frag += partitions[i];
        }
    }

    printf("\n\t========== %s ==========\n", title);
    printf("---------------------------------------------------------------\n");
    printf("Process No\tProcess Size\tBlock No\tBlock Size\tFragment\n");
    printf("---------------------------------------------------------------\n");

    for(i = 0; i < num_processes; i++) {
        if(alloc[i] != -1) {
            printf(" %d\t\t %d\t\t %d\t\t %d\t\t %d\n",
                   i+1, processes[i], alloc[i]+1, partitions[alloc[i]],
                   partitions[alloc[i]] - processes[i]);
        } else {
            printf(" %d\t\t %d\t\t Not Allocated\n", i+1, processes[i]);
        }
    }

    printf("---------------------------------------------------------------\n");
    printf("Total Internal Fragmentation : %d\n", internal_frag);
    printf("Total External Fragmentation : %d\n", external_frag);
    printf("---------------------------------------------------------------\n");
}

/* First Fit Allocation */
void firstFit(int partitions[], int num_partitions, int processes[], int num_processes) {
    int i, j;
    int alloc[MAX];
    int part_used[MAX] = {0};

    for(i = 0; i < num_processes; i++) {
        alloc[i] = -1;
    }

    for(i = 0; i < num_processes; i++) {
        for(j = 0; j < num_partitions; j++) {
            if(!part_used[j] && partitions[j] >= processes[i]) {
                alloc[i] = j;
                part_used[j] = 1;
                break;
            }
        }
    }

    displayResults(partitions, num_partitions, processes, num_processes, alloc, "FIRST FIT");
}

/* Best Fit Allocation */
void bestFit(int partitions[], int num_partitions, int processes[], int num_processes) {
    int i, j;
    int alloc[MAX];
    int part_used[MAX] = {0};

    for(i = 0; i < num_processes; i++) {
        alloc[i] = -1;
    }

    for(i = 0; i < num_processes; i++) {
        int best_idx = -1;
        int min_diff = INT_MAX;

        for(j = 0; j < num_partitions; j++) {
            if(!part_used[j] && partitions[j] >= processes[i]) {
                int diff = partitions[j] - processes[i];
                if(diff < min_diff) {
                    min_diff = diff;
                    best_idx = j;
                }
            }
        }

        if(best_idx != -1) {
            alloc[i] = best_idx;
            part_used[best_idx] = 1;
        }
    }

    displayResults(partitions, num_partitions, processes, num_processes, alloc, "BEST FIT");
}

/* Worst Fit Allocation */
void worstFit(int partitions[], int num_partitions, int processes[], int num_processes) {
    int i, j;
    int alloc[MAX];
    int part_used[MAX] = {0};

    for(i = 0; i < num_processes; i++) {
        alloc[i] = -1;
    }

    for(i = 0; i < num_processes; i++) {
        int worst_idx = -1;
        int max_diff = -1;

        for(j = 0; j < num_partitions; j++) {
            if(!part_used[j] && partitions[j] >= processes[i]) {
                int diff = partitions[j] - processes[i];
                if(diff > max_diff) {
                    max_diff = diff;
                    worst_idx = j;
                }
            }
        }

        if(worst_idx != -1) {
            alloc[i] = worst_idx;
            part_used[worst_idx] = 1;
        }
    }

    displayResults(partitions, num_partitions, processes, num_processes, alloc, "WORST FIT");
}

int main() {
    int partitions[MAX], processes[MAX];
    int num_partitions, num_processes;
    int i, choice;

    printf("\n===== CONTIGUOUS MEMORY ALLOCATION =====\n");

    /* Input memory partitions */
    printf("\nEnter the number of Memory Partitions: ");
    scanf("%d", &num_partitions);

    printf("Enter the size of each Memory Partition:\n");
    for(i = 0; i < num_partitions; i++) {
        printf("Partition %d: ", i+1);
        scanf("%d", &partitions[i]);
    }

    /* Input processes */
    printf("\nEnter the number of Processes: ");
    scanf("%d", &num_processes);

    printf("Enter the size of each Process:\n");
    for(i = 0; i < num_processes; i++) {
        printf("Process %d: ", i+1);
        scanf("%d", &processes[i]);
    }

    while(1) {
        printf("\n========== MENU ==========\n");
        printf("1. First Fit\n");
        printf("2. Best Fit\n");
        printf("3. Worst Fit\n");
        printf("4. Exit\n");
        printf("==========================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                firstFit(partitions, num_partitions, processes, num_processes);
                break;
            case 2:
                bestFit(partitions, num_partitions, processes, num_processes);
                break;
            case 3:
                worstFit(partitions, num_partitions, processes, num_processes);
                break;
            case 4:
                printf("\nExiting program. Thank you!\n");
                exit(0);
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
    }

    return 0;
}
