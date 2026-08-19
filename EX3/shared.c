#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int shmid, *buf;
    int i, num, size;

    // Cleaned prompt without arrows
    printf("Shared Memory Size Required: ");
    if (scanf("%d", &size) != 1 || size <= 0) 
    {
        printf("[ERROR] Invalid memory size provided.\n");
        exit(EXIT_FAILURE);
    }

    // Create shared memory
    shmid = shmget(4777, sizeof(int) * size, IPC_CREAT | 0666);

    if (shmid == -1)
    {
        perror("[SYSTEM ERROR]");
        printf("[FAILURE] Could not allocate shared memory segment.\n");
        exit(EXIT_FAILURE);
    }

    printf("[SUCCESS] Shared memory segment allocated.\n");

    // Attach shared memory
    buf = (int *)shmat(shmid, NULL, 0);

    if (buf == (void *)-1)
    {
        perror("[SYSTEM ERROR]");
        printf("[FAILURE] Connection to shared memory failed.\n");
        exit(EXIT_FAILURE);
    }

    printf("[SUCCESS] Attached to shared memory address space.\n");

    // Fork the process
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("[SYSTEM ERROR]");
        printf("[FAILURE] Fork operation failed.\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        // Child/Producer Interface
        printf("\n-----------------------------------------\n");
        printf(" MODE: PRODUCER PROCESS (CHILD)\n");
        printf("-----------------------------------------\n");

        for (i = 0; i < size; i++)
        {
            printf(" Input value [%d/%d]: ", i + 1, size);
            scanf("%d", &num);
            buf[i] = num;
        }

        printf("\n[STATUS] Data injection complete.\n");

        // Detach from shared memory
        if (shmdt(buf) == -1)
        {
            perror("[SYSTEM ERROR]");
            printf("[WARNING] Producer could not cleanly detach.\n");
        }
        else
        {
            printf("[STATUS] Producer detached safely.\n");
        }

        exit(0);
    }
    else
    {
        wait(NULL);

        // Parent/Consumer Interface
        printf("\n-----------------------------------------\n");
        printf(" MODE: CONSUMER PROCESS (PARENT)\n");
        printf("-----------------------------------------\n");
        printf("Reading values from shared memory block:\n");

        for (i = 0; i < size; i++)
        {
            printf(" Shared Memory Slot [%d]: %d\n", i + 1, buf[i]);
        }

        // Detach from shared memory
        if (shmdt(buf) == -1)
        {
            perror("[SYSTEM ERROR]");
            printf("[WARNING] Consumer could not cleanly detach.\n");
        }
        else
        {
            printf("\n[STATUS] Consumer detached safely.\n");
        }

        // Clean up and delete the shared memory segment
        if (shmctl(shmid, IPC_RMID, NULL) == -1)
        {
            perror("[SYSTEM ERROR]");
            printf("[FAILURE] Memory deallocation block failed.\n");
        }
        else
        {
            printf("[SUCCESS] Shared memory resource completely freed.\n");
        }
    }

    return 0;
}
