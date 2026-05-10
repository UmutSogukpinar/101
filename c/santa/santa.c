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

// Indicates whether an error occurred during the simulation.
int g_error_flag = 0;

// Stores the number of reindeer currently waiting for Santa.
int g_reindeer_waiting = 0;

// Stores the number of elves currently waiting for Santa's help.
int g_elves_waiting = 0;

// Stores the starting time of the simulation.
time_t g_milestone = 0;

// Prevents multiple threads from printing at the same time.
mutex lock_print = PTHREAD_MUTEX_INITIALIZER;

// Protects access to the shared elf counter.
mutex lock_elves = PTHREAD_MUTEX_INITIALIZER;

// Protects access to the global error flag.
mutex lock_error = PTHREAD_MUTEX_INITIALIZER;

// Protects access to the shared reindeer counter.
mutex lock_reindeer = PTHREAD_MUTEX_INITIALIZER;

// Used to wake Santa when elves or reindeer need him.
sem_t santa_sem;

// Limits the number of elves that can wait for Santa at the same time.
sem_t elf_sem;

// Controls reindeer participation in the synchronization cycle.
sem_t reindeer_sem;

// Blocks elves until Santa finishes helping their group.
sem_t elf_barrier;

// Blocks reindeer until Santa is ready to prepare the sleigh.
sem_t reindeer_barrier;

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

/**
 * @brief Suspends the current thread for the given number of seconds.
 *
 * @param seconds Number of seconds to sleep.
 */
void spend_time(int seconds)
{
    sleep(seconds);
}

// ================== SEMAPHORE FUNCTIONS ==================

/**
 * @brief Opens a semaphore barrier for a fixed number of waiting threads.
 *
 * This function posts to the given semaphore `count` times, allowing exactly
 * `count` blocked threads to continue execution.
 *
 * @param barrier Pointer to the semaphore barrier to open.
 * @param count Number of threads to release.
 */
void open_barrier(sem_t *barrier, int count)
{
    for (int i = 0; i < count; ++i) 
    {
        sem_post(barrier);
    }
}

// ================== MUTEX FUNCTIONS ==================

/**
 * @brief Updates a shared counter in a thread-safe way.
 *
 * This function locks the given mutex, applies the requested change to the
 * shared counter, and unlocks the mutex.
 *
 * @param lock Mutex protecting the shared counter.
 * @param counter Pointer to the shared counter.
 * @param change Value to add to the counter.
 */
static void	change_waiting_count(mutex *lock, int *counter, int change)
{
	pthread_mutex_lock(lock);
	*counter += change;
	pthread_mutex_unlock(lock);
}

/**
 * @brief Wake up Santa if enough actors are waiting, then waits on a barrier.
 *
 * @param lock Mutex protecting the shared waiting counter.
 * @param waiting_count Pointer to the shared waiting counter.
 * @param required_count Required number of waiting actors.
 * @param barrier Barrier semaphore used to block the current actor.
 */
static void	check_waiting_status(
	mutex *lock,
	int *waiting_count,
	int required_count,
	sem_t *barrier
)
{
	pthread_mutex_lock(lock);

	if (*waiting_count >= required_count)
		sem_post(&santa_sem);

	pthread_mutex_unlock(lock);

	sem_wait(barrier);
}

// ================== PRINTING FUNCTIONS ==================

/**
 * @brief Prints a simulation action in a thread-safe way.
 *
 * @param data Pointer to the related actor data, or NULL for Santa-only actions.
 * @param type Type of action to print.
 */
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


/**
 * @brief Main routine executed by each reindeer thread.
 *
 * A reindeer waits for permission to join the group, increments the waiting
 * reindeer counter, waits until Santa releases the reindeer barrier, prints
 * its action, and then sleeps before repeating the cycle.
 *
 * @param arg Pointer to a t_reindeer structure.
 * @return Always returns NULL.
 */
static void *do_reindeer_work(void *arg)
{
    t_reindeer* reindeer = (t_reindeer *)arg;

    while (true)
    {
        sem_wait(&reindeer_sem);
        change_waiting_count(&lock_reindeer, &g_reindeer_waiting, +1);

        check_waiting_status(
            &lock_reindeer, &g_reindeer_waiting,
            REQUIRED_REINDEER, &reindeer_barrier
        );

        do_action(reindeer, ACTION_REINDEER);

        sem_post(&reindeer_sem);
        spend_time(REINDEER_SLEEP);
    }

    return (NULL);
}

/**
 * @brief Main routine executed by each elf thread.
 *
 * An elf waits for permission to join a group of elves, increments the waiting
 * elf counter, waits until Santa releases the elf barrier, prints its action,
 * and then sleeps before repeating the cycle.
 *
 * @param arg Pointer to a t_elf structure.
 * @return Always returns NULL.
 */
static void *do_elves_work(void *arg)
{
    t_elf* elf = (t_elf *)arg;

    while (true)
    {
        sem_wait(&elf_sem);
        change_waiting_count(&lock_elves, &g_elves_waiting, +1);

        check_waiting_status(
            &lock_elves, &g_elves_waiting,
            REQUIRED_ELVES, &elf_barrier
        );

        do_action(elf, ACTION_ELF);

        sem_post(&elf_sem);
        spend_time(ELVES_SLEEP);
    }

    return (NULL);
}

/**
 * @brief Main routine executed by the Santa thread.
 *
 * Santa sleeps until notified by elves or reindeer. Reindeer have priority:
 * if all reindeer are waiting, Santa prepares the sleigh and releases them.
 * Otherwise, if enough elves are waiting, Santa helps the elf group and
 * releases them.
 *
 * @param arg Unused argument.
 * @return Always returns NULL.
 */
static void *do_santa_work(void *arg)
{
    (void) arg;

    while (true)
    {
        sem_wait(&santa_sem);

        pthread_mutex_lock(&lock_reindeer);
        if (g_reindeer_waiting >= REQUIRED_REINDEER)
        {
            g_reindeer_waiting -= REQUIRED_REINDEER;
            do_action(NULL, ACTION_SANTA_WITH_REINDEER);
            open_barrier(&reindeer_barrier, REQUIRED_REINDEER);
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

/**
 * @brief Initializes all semaphores used by the simulation.
 *
 * This function initializes Santa, elf, reindeer, and barrier semaphores.
 * If any initialization fails, previously initialized semaphores are destroyed
 * before returning failure.
 *
 * @return SUCCESS on successful initialization, FAILURE otherwise.
 */
static int init_sems(void)
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

    if (sem_init(&reindeer_barrier, 0, 0) == FAILURE) 
    {
        perror("reindeer_barrier failed");
        sem_destroy(&elf_barrier);
        sem_destroy(&reindeer_sem);
        sem_destroy(&elf_sem);
        sem_destroy(&santa_sem);
        return (FAILURE);
    }

    return (SUCCESS);
}

/**
 * @brief Initializes the simulation start timestamp.
 *
 * The global milestone is used as the reference point for calculating elapsed
 * time in printed simulation messages.
 */
static void init_milestone(void)
{
    g_milestone = time(NULL);
}


/**
 * @brief Creates a thread based on the given action type.
 *
 * This function selects the correct thread routine according to the actor type
 * and starts the corresponding pthread.
 *
 * @param type Actor type that determines which routine will be used.
 * @param data Pointer to the actor structure.
 */
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

/**
 * @brief Initializes and starts the Santa Claus simulation.
 *
 * This function initializes semaphores and the simulation timestamp, assigns
 * IDs to elves and reindeer, creates all actor threads, and waits for the
 * Santa thread.
 *
 * @param elves Array of elf structures.
 * @param reindeer Array of reindeer structures.
 * @param santa Pointer to the Santa structure.
 */
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
