#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include "threadpool.h"

#define TASK_NUM	16
#define THREAD_NUM	8
#define SIZE		10000
#define COUNT		10000

struct timeval start, end;

typedef struct {
    int count;
    unsigned long size;
} arg;

static tpool_t *pool = NULL;

void allocate(void *data){
    arg *p = (arg *) data;
    int i;
    int **m = (int **)malloc(p->count * sizeof(void *));

    for (i = 0; i < p->count; i++)
        m[i] = (int *)malloc(p->size);
    for (i = 0; i < p->count; i++)
        free(m[i]);
    free(m);
}

static void *task_run(void *data __attribute__ ((__unused__)))
{
    while(1) {
        task_t *_task = tqueue_pop(pool->queue);
        if (_task) {
            if (!_task->func) {
                tqueue_push(pool->queue, _task);
                break;
            } else {
                _task->func(_task->arg);
                free(_task);
            }
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int i;
    int task_num = TASK_NUM;
    int thread_num = THREAD_NUM;
    double duration;

    arg *the_arg = (arg *) malloc(sizeof(arg));
    the_arg->count = COUNT;
    the_arg->size = SIZE;

    pool = (tpool_t *) malloc(sizeof(tpool_t));
    tpool_init(pool, thread_num, task_run);

    gettimeofday(&start, NULL);
    for (i = 0; i < task_num; i++)
        tqueue_push(pool->queue, task_new(allocate, the_arg));

    tqueue_push(pool->queue, task_new(NULL, NULL));

    tpool_free(pool);

    gettimeofday(&end, NULL);
    duration = (end.tv_sec - start.tv_sec) * 1000 +
               (double)(end.tv_usec - start.tv_usec) / 1000.0f;
    printf("%f\n", duration);

    return 0;
}

