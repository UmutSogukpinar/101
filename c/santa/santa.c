#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#include <unistd.h>
#include <time.h>

typedef pthread_mutex_t mutex;

// ==================== CONSTANTS ====================

# define FAILURE -1
# define SUCCESS 0

// ==================== CONFIGURATION ====================

# define ELVES_COUNT 6
# define SANTA_COUNT 1
# define REINDEER_COUNT 9

# define REQUIRED_REINDEER 9
# define REQUIRED_ELVES 3

# define REINDEER_SLEEP 50
# define ELVES_SLEEP 15
# define SANTA_SLEEP 10

// ==========f========== GLOBAL VARIABLES ====================

int g_error_flag = 0;
int g_reindeer_waiting = 0;
int g_elves_waiting = 0;

time_t g_milestone = 0;

mutex lock_print = PTHREAD_MUTEX_INITIALIZER;
mutex lock_elves = PTHREAD_MUTEX_INITIALIZER;
mutex lock_error = PTHREAD_MUTEX_INITIALIZER;
mutex lock_reindeer = PTHREAD_MUTEX_INITIALIZER;

sem_t santa_sem;
sem_t elf_sem;
sem_t reindeer_sem;
sem_t elf_barrier;

// ==================== ENUMS ====================

typedef enum e_action_type
{
    ACTION_SANTA_WITH_ELVES,
    ACTION_SANTA_WITH_REINDEER,
    ACTION_ELF,
    ACTION_REINDEER,
    ACTION_SANTA

}	t_action_type;

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

// ================== TIME FUNCTIONS ==================

void spend_time(int seconds)
{
    sleep(seconds);
}

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
        sem_post(&santa_sem);
    }

    pthread_mutex_unlock(&lock_elves);
    
    sem_wait(&elf_barrier);
}

void change_reindeer_waiting(int change)
{
    pthread_mutex_lock(&lock_reindeer);

    g_reindeer_waiting += change;
    
    pthread_mutex_unlock(&lock_reindeer);
}

void check_reindeer_status()
{
    pthread_mutex_lock(&lock_reindeer);

    if (g_reindeer_waiting >= REQUIRED_REINDEER)
    {
        sem_post(&santa_sem);
    }

    pthread_mutex_unlock(&lock_reindeer);
}

// ================== PRINTING FUNCTIONS ==================

static void do_action(void *data, t_action_type type)
{
    time_t current_time = time(NULL);

	pthread_mutex_lock(&lock_print);

    time_t elapsed_time = current_time - g_milestone;

	switch (type)
	{
		case ACTION_ELF:
		{
			t_elf *elf = (t_elf *)data;
			printf("%ld: Elf %zu is getting help from Santa.\n", elapsed_time, elf->id);
			break;
		}

        case ACTION_REINDEER:
        {
            t_reindeer *reindeer = (t_reindeer *)data;
            printf("%ld: Reindeer %zu is waiting for Santa.\n", elapsed_time, reindeer->id);
            break;
        }

		case ACTION_SANTA_WITH_REINDEER:
		{
			printf("%ld: Santa is hitting the road with %d reindeer!\n", elapsed_time, REQUIRED_REINDEER);
			break;
		}

        case ACTION_SANTA_WITH_ELVES:
        {
            printf("%ld: Santa is helping the elves and preparing the sleigh.\n", elapsed_time);
            break;
        }

		default:
			break;
	}

	pthread_mutex_unlock(&lock_print);
}

// ==================== THREAD FUNCTIONS ====================


static void* do_reindeer_work(void *arg)
{
    t_reindeer* reindeer = (t_reindeer *)arg;

    while (true)
    {
        sem_wait(&reindeer_sem);
        change_reindeer_waiting(+1);

        check_reindeer_status();

        do_action(reindeer, ACTION_REINDEER);

        sem_post(&reindeer_sem);
        spend_time(REINDEER_SLEEP);
    }

    return (NULL);
}

static void* do_elves_work(void *arg)
{
    t_elf* elf = (t_elf *)arg;

    while (true)
    {
        sem_wait(&elf_sem);
        change_elves_waiting(+1);

        check_elves_status();

        do_action(elf, ACTION_ELF);

        sem_post(&elf_sem);
        spend_time(ELVES_SLEEP);
    }

    return (NULL);
}

static void* do_santa_work(void *arg)
{
    (void)arg;

    while (true)
    {
        sem_wait(&santa_sem);

        pthread_mutex_lock(&lock_reindeer);
        if (g_reindeer_waiting >= REQUIRED_REINDEER)
        {
            do_action(NULL, ACTION_SANTA_WITH_REINDEER);
            g_reindeer_waiting -= REQUIRED_REINDEER;
            pthread_mutex_unlock(&lock_reindeer);
            spend_time(SANTA_SLEEP);

            continue;
        }

        pthread_mutex_unlock(&lock_reindeer);

        pthread_mutex_lock(&lock_elves);
        if (g_elves_waiting >= REQUIRED_ELVES)
        {
            do_action(NULL, ACTION_SANTA_WITH_ELVES);
            g_elves_waiting -= REQUIRED_ELVES; 
            open_barrier(&elf_barrier, REQUIRED_ELVES);
        }
        pthread_mutex_unlock(&lock_elves);
        
        spend_time(SANTA_SLEEP);
    }

    return (NULL);
}

// ==================== INITIALIZATION FUNCTIONS ====================

static int init_sems()
{
    if (sem_init(&santa_sem, 0, 0) == FAILURE) 
    {
        perror("santa_sem failed");
        return (FAILURE);
    }

    if (sem_init(&elf_sem, 0, REQUIRED_ELVES) == FAILURE)
    {
        perror("elf_sem failed");
        sem_destroy(&santa_sem);
        return (FAILURE);
    }

    if (sem_init(&reindeer_sem, 0, REQUIRED_REINDEER) == FAILURE) 
    {
        perror("reindeer_sem failed");
        sem_destroy(&santa_sem);
        sem_destroy(&elf_sem);
        return (FAILURE);
    }

    if (sem_init(&elf_barrier, 0, 0) == FAILURE)
    {
        perror("elf_barrier failed");
        sem_destroy(&reindeer_sem);
        sem_destroy(&elf_sem);
        sem_destroy(&santa_sem);
        return (FAILURE);
    }

    return (SUCCESS); 

}

static void init_milestone()
{
    g_milestone = time(NULL);
}

static void init_thread(t_action_type type, void *data)
{
    pthread_t *thread;

    switch (type)
    {
        case ACTION_ELF:
        {
            t_elf *elf = (t_elf *)data;
            thread = &elf->thread;
            pthread_create(thread, NULL, do_elves_work, data);
            break;
        }

        case ACTION_REINDEER:
        {
            t_reindeer *reindeer = (t_reindeer *)data;
            thread = &reindeer->thread;
            pthread_create(thread, NULL, do_reindeer_work, data);
            break;
        }

        case ACTION_SANTA:
        {
            t_santa *santa = (t_santa *)data;
            thread = &santa->thread;
            pthread_create(thread, NULL, do_santa_work, data);
            break;
        }

        default:
            break;
    }
}

void init_sim(t_elf *elves, t_reindeer *reindeer, t_santa *santa)
{
    if (init_sems() == FAILURE) exit(EXIT_FAILURE);
        
    init_milestone();

    for (int i = 0; i < ELVES_COUNT; ++i)
    {
        elves[i].id = i + 1;
        init_thread(ACTION_ELF, &elves[i]);
    }

    for (int i = 0; i < REINDEER_COUNT; ++i)
    {
        reindeer[i].id = i + 1;
        init_thread(ACTION_REINDEER, &reindeer[i]);
    }

    init_thread(ACTION_SANTA, santa);

    pthread_join(santa->thread, NULL);
}

int main()
{
    t_elf elves[ELVES_COUNT] = {0};
    t_reindeer reindeer[REINDEER_COUNT] = {0};
    t_santa santa = {0};

    init_sim(elves, reindeer, &santa);

    return (0);
}