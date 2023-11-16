#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_THREADS 100

sem_t sem_even, sem_odd;
pthread_mutex_t mutex;
int even_count = 0, odd_count = 0;

void thread_func(voidarg) {
    int id = (int) arg;
    int is_even = id % 2 == 0;

    // Wait for the semaphore
    if (is_even) {
        sem_wait(&sem_even);
    } else {
        sem_wait(&sem_odd);
    }

    // Enter critical section
    pthread_mutex_lock(&mutex);
    printf("Thread %d entered critical section\n", id);
    pthread_mutex_unlock(&mutex);

    // Sleep for a random amount of time
    int sleep_time = rand() % 5 + 1;
    sleep(sleep_time);

    // Exit critical section
    pthread_mutex_lock(&mutex);
    printf("Thread %d exited critical section\n", id);
    pthread_mutex_unlock(&mutex);

    // Release the semaphore
    if (is_even) {
        even_count++;
        if (even_count == odd_count) {
            sem_post(&sem_odd);
        } else {
            sem_post(&sem_even);
        }
    } else {
        odd_count++;
        if (even_count == odd_count) {
            sem_post(&sem_even);
        } else {
            sem_post(&sem_odd);
        }
    }

    return NULL;
}

int main() {
    int num_threads;
    scanf("%d", &num_threads);

    sem_init(&sem_even, 0, 0);
    sem_init(&sem_odd, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_t threads[MAX_THREADS];
    int thread_ids[MAX_THREADS];

    // Create the threads
    for (int i = 0; i < num_threads; i++) {
        scanf("%d", &thread_ids[i]);
        pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
    }

    // Start the first thread
    sem_post(&sem_even);

    // Wait for the threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&sem_even);
    sem_destroy(&sem_odd);
    pthread_mutex_destroy(&mutex);

    return 0;
}
