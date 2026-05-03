#ifndef SANTA_H
# define SANTA_H

#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

typedef pthread_mutex_t mutex;
typedef unsigned int t_ms;

// ==================== CONFIGURATION ====================

# define REINDEER_COUNT 9
# define ELVES_COUNT 6
# define SANTA_COUNT 1

# define REQUIRED_REINDEER 9
# define REQUIRED_ELVES 3

# define REINDEER_SLEEP 5000
# define ELVES_SLEEP 3000
# define SANTA_SLEEP 1000

// ==================== STRUCTURES ====================

typedef struct s_reindeer 
{
    pthread_t thread;
    size_t    id;
    bool      is_sleeping;

}   t_reindeer;

typedef struct s_elf
{
    pthread_t thread;
    size_t    id;
    bool      is_sleeping;

}   t_elf;

typedef struct s_santa
{
    pthread_t thread;

}   t_santa;

// ==================== GLOBAL VARIABLES ====================

extern int g_error_flag;
extern int g_reindeer_waiting;
extern int g_elves_waiting;

extern mutex lock_print;
extern mutex lock_error;
extern mutex lock_reindeer;
extern mutex lock_elves;

extern sem_t santa_sem;
extern sem_t elf_sem;
extern sem_t reindeer_sem;

extern sem_t elf_barrier;

// ==================== STATUS ====================

# define FAILURE -1


// ====================  FUNCTIONS ====================

void init_sim(t_elf *elves, t_reindeer *reindeer, t_santa *santa);

void *elf_thread(void *arg);
void *reindeer_thread(void *arg);


#endif