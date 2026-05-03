#include "santa.h"
#include <stdlib.h>

static void init_sems()
{
    if (sem_init(&santa_sem, 0, 0) == FAILURE)
    {
        perror("Failed to initialize semaphore");
        exit(EXIT_FAILURE);
    }

    if (sem_init(&elf_sem, 0, REQUIRED_ELVES) == FAILURE)
    {
        perror("Failed to initialize semaphore");
        sem_close(&santa_sem);
        exit(EXIT_FAILURE);
    }

    if (sem_init(&reindeer_sem, 0, REQUIRED_REINDEER) == FAILURE)
    {
        perror("Failed to initialize semaphore");
        sem_close(&santa_sem);
        sem_close(&elf_sem);
        exit(EXIT_FAILURE);
    }
}

void init_sim(t_elf *elves, t_reindeer *reindeer, t_santa *santa)
{
    init_sems();

    for (int i = 0; i < ELVES_COUNT; ++i)
    {
        if (pthread_create(&elves[i].thread, NULL,
            elf_thread, &elves[i]) != 0
        )
        {
            perror("Failed to create elf thread");
            exit(EXIT_FAILURE);
     
        }
    }

    // for (int i = 0; i < REINDEER_COUNT; ++i)
    // {
    //     if (pthread_create(&reindeer[i].thread, NULL,
    //         reindeer_thread, &reindeer[i]) != 0
    //     )
    //     {
    //         perror("Failed to create reindeer thread");
    //         exit(EXIT_FAILURE);
    //     }
    // }

    pthread_join(santa->thread, NULL);
}
