[24bcs056@mepcolinux ex7]$cat q1.c
#include <stdio.h>
#include <stdbool.h>

#define LIMIT 20

// Renamed structure and fields
struct Partition {
    int tag;         // Original index
    int capacity;    // Initial size
    int vacant;      // Free space left
};

// Reorganized printing function with modified table styling
void printMetrics(struct Partition parts[], int part_total, int jobs[], int job_total, int mappings[]) {
    int inside_frag = 0;
    int outside_frag = 0;
    bool holds_skipped = false;
    int untouched_memory = 0;

    printf("\nJob ID\t\tJob Volume\tPartition Tag\tInternal Leak\n");
    printf("=============================================================\n");
    
    int index = 0;
    while (index < job_total) {
        printf("  J%-10d\t  %-12d\t", index + 1, jobs[index]);
        
        if (mappings[index] != -1) {
            int slot = mappings[index];
            int gap = parts[slot].vacant;
            printf("  Slot %-5d\t  %-12d\n", parts[slot].tag, gap);
        } else {
            printf("  Unassigned\t  -\n");
            holds_skipped = true;
        }
        index++;
    }

    for (int k = 0; k < part_total; k++) {
        if (parts[k].vacant < parts[k].capacity) {
            inside_frag += parts[k].vacant;
        } else {
            untouched_memory += parts[k].capacity;
        }
    }

    if (holds_skipped) {
        outside_frag = untouched_memory;
    }

    printf("=============================================================\n");
    printf("Cumulative Internal Fragmentation : %d\n", inside_frag);
    printf("Cumulative External Fragmentation : %d\n", outside_frag);
}

// Rewritten sorting logic using a different bubble sort design pattern
void orderPartitions(struct Partition parts[], int count, int sequence[], int strategy) {
    int a = 0;
    do {
        sequence[a] = a;
        a++;
    } while (a < count);

    if (strategy == 1) return;

    int scan_a = 0;
    while (scan_a < count - 1) {
        int scan_b = 0;
        while (scan_b < count - scan_a - 1) {
            bool triggers_swap = false;
            
            if (strategy == 2) {
                triggers_swap = (parts[sequence[scan_b]].vacant > parts[sequence[scan_b + 1]].vacant);
            } else if (strategy == 3) {
                triggers_swap = (parts[sequence[scan_b]].vacant < parts[sequence[scan_b + 1]].vacant);
            }

            if (triggers_swap) {
                int backup = sequence[scan_b];
                sequence[scan_b] = sequence[scan_b + 1];
                sequence[scan_b + 1] = backup;
            }
            scan_b++;
        }
        scan_a++;
    }
}

// Consolidated execution engine using while-loops and updated local arrays
void runAllocationEngine(struct Partition source_parts[], int part_total, int jobs[], int job_total, int strategy) {
    struct Partition local_parts[LIMIT];
    int mappings[LIMIT];
    int sequence[LIMIT];

    for (int m = 0; m < part_total; m++) {
        local_parts[m] = source_parts[m];
        local_parts[m].vacant = source_parts[m].capacity;
    }
    
    for (int n = 0; n < job_total; n++) {
        mappings[n] = -1;
    }

    int job_idx = 0;
    while (job_idx < job_total) {
        orderPartitions(local_parts, part_total, sequence, strategy);

        int search_idx = 0;
        while (search_idx < part_total) {
            int current_slot = sequence[search_idx];
            
            if (local_parts[current_slot].vacant >= jobs[job_idx]) {
                mappings[job_idx] = current_slot;
                local_parts[current_slot].vacant -= jobs[job_idx];
                break;
            }
            search_idx++;
        }
        job_idx++;
    }

    printMetrics(local_parts, part_total, jobs, job_total, mappings);
}

// Wrapper routines mapping directly to the underlying engine
void executeFirstFit(struct Partition source[], int p_count, int requests[], int r_count) {
    runAllocationEngine(source, p_count, requests, r_count, 1);
}

void executeBestFit(struct Partition source[], int p_count, int requests[], int r_count) {
    runAllocationEngine(source, p_count, requests, r_count, 2);
}

void executeWorstFit(struct Partition source[], int p_count, int requests[], int r_count) {
    runAllocationEngine(source, p_count, requests, r_count, 3);
}

int main() {
    int total_slots, total_jobs, menu_select;
    struct Partition memory_slots[LIMIT];
    int job_requests[LIMIT];

    printf("Provide total memory segments: ");
    scanf("%d", &total_slots);
    
    printf("Provide capacities for segments:\n");
    for (int idx = 0; idx < total_slots; idx++) {
        memory_slots[idx].tag = idx + 1;
        printf("Segment %d: ", idx + 1);
        scanf("%d", &memory_slots[idx].capacity);
        memory_slots[idx].vacant = memory_slots[idx].capacity;
    }

    printf("\nProvide total customer jobs: ");
    scanf("%d", &total_jobs);
    
    printf("Provide capacity demands for jobs:\n");
    for (int idx = 0; idx < total_jobs; idx++) {
        printf("Job J%d: ", idx + 1);
        scanf("%d", &job_requests[idx]);
    }

    while (true) {
        printf("\n-----------------------------------------\n");
        printf("         SYSTEM REALLOCATION HUB         \n");
        printf("-----------------------------------------\n");
        printf("1] Run First-Fit Engine\n");
        printf("2] Run Best-Fit Engine\n");
        printf("3] Run Worst-Fit Engine\n");
        printf("4] Terminate Program\n");
        printf("Select an operation: ");
        scanf("%d", &menu_select);

        if (menu_select == 1) {
            printf("\n--- RUNNING FIRST-FIT MODE ---");
            executeFirstFit(memory_slots, total_slots, job_requests, total_jobs);
        } else if (menu_select == 2) {
            printf("\n--- RUNNING BEST-FIT MODE ---");
            executeBestFit(memory_slots, total_slots, job_requests, total_jobs);
        } else if (menu_select == 3) {
            printf("\n--- RUNNING WORST-FIT MODE ---");
            executeWorstFit(memory_slots, total_slots, job_requests, total_jobs);
        } else if (menu_select == 4) {
            break;
        } else {
            printf("Selection out of bounds. Retry.\n");
        }
    }

    return 0;
}

[24bcs056@mepcolinux ex7]$./q1
Provide total memory segments: 5
Provide capacities for segments:
Segment 1: 375
Segment 2: 400
Segment 3: 200
Segment 4: 175
Segment 5: 250

Provide total customer jobs: 5
Provide capacity demands for jobs:
Job J1: 300
Job J2: 380
Job J3: 200
Job J4: 180
Job J5: 200

-----------------------------------------
         SYSTEM REALLOCATION HUB         
-----------------------------------------
1] Run First-Fit Engine
2] Run Best-Fit Engine
3] Run Worst-Fit Engine
4] Terminate Program
Select an operation: 1

--- RUNNING FIRST-FIT MODE ---
Job ID		Job Volume	Partition Tag	Internal Leak
=============================================================
  J1         	  300         	  Slot 1    	  75          
  J2         	  380         	  Slot 2    	  20          
  J3         	  200         	  Slot 3    	  0           
  J4         	  180         	  Slot 5    	  70          
  J5         	  200         	  Unassigned	  -
=============================================================
Cumulative Internal Fragmentation : 165
Cumulative External Fragmentation : 175

-----------------------------------------
         SYSTEM REALLOCATION HUB         
-----------------------------------------
1] Run First-Fit Engine
2] Run Best-Fit Engine
3] Run Worst-Fit Engine
4] Terminate Program
Select an operation: 2

--- RUNNING BEST-FIT MODE ---
Job ID		Job Volume	Partition Tag	Internal Leak
=============================================================
  J1         	  300         	  Slot 1    	  75          
  J2         	  380         	  Slot 2    	  20          
  J3         	  200         	  Slot 3    	  0           
  J4         	  180         	  Slot 5    	  70          
  J5         	  200         	  Unassigned	  -
=============================================================
Cumulative Internal Fragmentation : 165
Cumulative External Fragmentation : 175

-----------------------------------------
         SYSTEM REALLOCATION HUB         
-----------------------------------------
1] Run First-Fit Engine
2] Run Best-Fit Engine
3] Run Worst-Fit Engine
4] Terminate Program
Select an operation: 3

--- RUNNING WORST-FIT MODE ---
Job ID		Job Volume	Partition Tag	Internal Leak
=============================================================
  J1         	  300         	  Slot 2    	  100         
  J2         	  380         	  Unassigned	  -
  J3         	  200         	  Slot 1    	  175         
  J4         	  180         	  Slot 5    	  70          
  J5         	  200         	  Slot 3    	  0           
=============================================================
Cumulative Internal Fragmentation : 345
Cumulative External Fragmentation : 175

-----------------------------------------
         SYSTEM REALLOCATION HUB         
-----------------------------------------
1] Run First-Fit Engine
2] Run Best-Fit Engine
3] Run Worst-Fit Engine
4] Terminate Program
Select an operation: 4
