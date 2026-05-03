#include <stdio.h>
#include <stdlib.h>

#include "santa.h"

// ==================== GLOBAL VARIABLES ====================

int g_error_flag = 0;
int g_reindeer_waiting = 0;
int g_elves_waiting = 0;

mutex lock_print = PTHREAD_MUTEX_INITIALIZER;
mutex lock_elves = PTHREAD_MUTEX_INITIALIZER;
mutex lock_error = PTHREAD_MUTEX_INITIALIZER;
mutex lock_reindeer = PTHREAD_MUTEX_INITIALIZER;


// ================== SEMAPHORE FUNCTIONS ==================

void open_barrier(sem_t *barrier, int count)
{
    for (int i = 0; i < count; ++i)
    {
        sem_post(barrier);
    }
}

// ================== MUTEX FUNCTIONS ==================

void change_elves_waiting(int change)
{
    pthread_mutex_lock(&lock_elves);

    g_elves_waiting += change;
    
    pthread_mutex_unlock(&lock_elves);
}

void check_elves_status()
{
    pthread_mutex_lock(&lock_elves);

    if (g_elves_waiting >= REQUIRED_ELVES)
    {
        pthread_mutex_unlock(&lock_elves);
        open_barrier(&elf_barrier, REQUIRED_ELVES);
        sem_post(&santa_sem);
    }
    else
    {
        pthread_mutex_unlock(&lock_elves);
    }
    
    sem_wait(&elf_barrier);
}

// ==================== THREAD FUNCTIONS ====================

void do_elves_action(t_elf *elf)
{
    pthread_mutex_lock(&lock_print);
    printf("Elf %d is getting help from Santa.\n", elf->id);
    pthread_mutex_unlock(&lock_print);
}

void elf_thread(void *arg)
{
    t_elf* elf = (t_elf *)arg;

    while (true)
    {
        sem_wait(&elf_sem);
        change_elves_waiting(+1);

        check_elves_status();

        do_elves_action(elf);

        sem_post(&elf_sem);
        spend_time(ELVES_SLEEP);
    }
}


int main()
{
    t_elf elves[ELVES_COUNT] = {0};
    t_reindeer reindeer[REINDEER_COUNT] = {0};
    t_santa santa = {0};

    init_sim(elves, reindeer, &santa);

    return (0);
}