#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>

pthread_mutex_t mutex;
#define MAX_RESOURCES 5
int available_resources = MAX_RESOURCES;

typedef struct thread {
    char tid[4];
    unsigned int startTime;
    int state;
    pthread_t handle;
    int retVal;
} Thread;

void logStart(char *tID) {
    printf("[%ld] New Thread with ID %s is started.\n", getCurrentTime(), tID);
}

void logFinish(char *tID) {
    printf("[%ld] Thread with ID %s is finished.\n", getCurrentTime(), tID);
}

void* threadRun(void *t) {
    Thread *currentThread = (Thread*) t;
    logStart(currentThread->tid);

    pthread_mutex_lock(&mutex); // Lock critical section for resource acquisition
    if (available_resources > 0) {
        available_resources--;
        printf("[%ld] Thread %s has acquired 1 resource. %d resource(s) remaining.\n",
               getCurrentTime(), currentThread->tid, available_resources);
        pthread_mutex_unlock(&mutex); // Unlock critical section after resource acquisition

        // Simulating work by sleeping for 1 second
        sleep(1);

        pthread_mutex_lock(&mutex); // Lock critical section for resource release
        available_resources++;
        printf("[%ld] Thread %s has released 1 resource. %d resource(s) available now.\n",
               getCurrentTime(), currentThread->tid, available_resources);
        pthread_mutex_unlock(&mutex); // Unlock critical section after resource release
    } else {
        printf("[%ld] Thread %s could not acquire enough resources.\n",
               getCurrentTime(), currentThread->tid);
        pthread_mutex_unlock(&mutex); // Unlock critical section if resources are insufficient
    }

    logFinish(currentThread->tid);
    currentThread->state = -1;
    pthread_exit(0);
}

// The rest of the code remains unchanged...

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Input file name missing...exiting with error code -1\n");
        return -1;
    }

    Thread *threads = NULL;
    int threadCount = readFile(argv[1], &threads);

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Mutex initialization failed.\n");
        return 1;
    }

    startClock();
    while (threadsLeft(threads, threadCount) > 0) {
        int i = 0;
        while ((i = threadToStart(threads, threadCount)) > -1) {
            threads[i].state = 1;
            threads[i].retVal = pthread_create(&(threads[i].handle), NULL,
                                               threadRun, &threads[i]);
        }
    }

    for (int i = 0; i < threadCount; ++i) {
        pthread_join(threads[i].handle, NULL);
    }

    pthread_mutex_destroy(&mutex);

    printf("All threads have finished execution. Available resources: %d\n", available_resources);

    return 0;
}