#include <pthread.h>
#include <chrono>
#include <iostream>
#include <cstring>
#include "../../common/common.h" 

struct Args {
    int tid;
    int num_threads;
    float *matrix;
    int N;
    pthread_barrier_t *barrier;
    int *counter;
    pthread_mutex_t *mtx;
    pthread_cond_t *cond;
    std::chrono::high_resolution_clock::time_point *start;
    std::chrono::high_resolution_clock::time_point *end;
};

void perf_cycle(int num_threads, int *counter, pthread_mutex_t *mtx,
                pthread_cond_t *cond,
                std::chrono::high_resolution_clock::time_point *time) {
    pthread_mutex_lock(mtx);

    *counter -= 1;
    if (*counter == 0) {
        *time = std::chrono::high_resolution_clock::now();
        *counter = num_threads;
        pthread_cond_broadcast(cond);
    } else {
        pthread_cond_wait(cond, mtx);
    }

    pthread_mutex_unlock(mtx);
}

void *ge_parallel(void *args) {
    Args *local_args = (Args *)args;

    int tid = local_args->tid;
    int num_threads = local_args->num_threads;
    float *matrix = local_args->matrix;
    int N = local_args->N;
    pthread_barrier_t *barrier = local_args->barrier;
    int *counter = local_args->counter;
    pthread_mutex_t *mtx = local_args->mtx;
    pthread_cond_t *cond = local_args->cond;
    std::chrono::high_resolution_clock::time_point *start = local_args->start;
    std::chrono::high_resolution_clock::time_point *end = local_args->end;

    perf_cycle(num_threads, counter, mtx, cond, start);

    for (int i = 0; i < N - 1; ++i) {
        if ((i % num_threads) == tid) {
            float pivot = matrix[i * N + i];
            for (int j = i + 1; j < N; ++j) {
                matrix[i * N + j] /= pivot;
            }
            matrix[i * N + i] = 1;
        }

        pthread_barrier_wait(barrier);

        for (int j = i + 1; j < N; ++j) {
            if ((j % num_threads) == tid) {
                float scale = matrix[j * N + i];
                for (int l = i + 1; l < N; ++l) {
                    matrix[j * N + l] -= matrix[i * N + l] * scale;
                }
                matrix[j * N + i] = 0;
            }
        }
    }

    if (tid == num_threads - 1) {
        matrix[(N - 1) * N + N - 1] = 1;
    }

    perf_cycle(num_threads, counter, mtx, cond, end);

    return 0;
}

void launch_threads(int num_threads, float* matrix, int N) {
    pthread_t *threads = new pthread_t[num_threads];
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, num_threads);

    Args thread_args[num_threads];

    int counter = num_threads;
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;

    for (int i = 0; i < num_threads; ++i) {
        thread_args[i].tid = i;
        thread_args[i].num_threads = num_threads;
        thread_args[i].matrix = matrix;
        thread_args[i].N = N;
        thread_args[i].barrier = &barrier;
        thread_args[i].counter = &counter;
        thread_args[i].mtx = &mtx;
        thread_args[i].cond = &cond;
        thread_args[i].start = &start;
        thread_args[i].end = &end;

        if (pthread_create(&threads[i], NULL, ge_parallel, (void*)&thread_args[i]) != 0) {
            std::cerr << "Error creating thread " << i << std::endl;
            exit(1);
        }
    }

    for (int i = 0; i < num_threads; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            std::cerr << "Error joining thread " << i << std::endl;
            exit(1);
        }
    }

    std::chrono::duration<double> elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    std::cout << "Elapsed time parallel = " << elapsed.count() << " seconds" << std::endl;

    pthread_barrier_destroy(&barrier);
    delete[] threads;
}