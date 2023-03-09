#include <iostream>
#include <cstdlib>
#include <cmath>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM_POINTS 1000
#define NUM_THREADS 6
double pi = 3.14159265358;
int h = 2;
unsigned long long int num_inside_points = 0;
unsigned long long int num_all_points = 0;

sem_t sem;

int rand_r(unsigned int *nextp) {
    *nextp = *nextp * 1103515245 + 12345;
    return (unsigned int) (*nextp / 65536) % 32768;
}

void *func(void *param) {
    int val;
    unsigned long long int local = *((unsigned long long int *) param);
    double x, y, z;
    sem_wait(&sem);
    for (unsigned long long int i = 0; i < (unsigned long long int) local; i++) {
        unsigned int x_k, y_k, z_k;
        x = ((float) rand_r(&x_k) / RAND_MAX) * pi;
        y = (float) rand_r(&y_k) / RAND_MAX;
        z = ((float) rand_r(&z_k) / RAND_MAX) * h;
        if (y <= sin(x) && z <= x * y)
            num_inside_points++;
        num_all_points++;
    }
    sem_post(&sem);
    pthread_exit(NULL);
    sem_getvalue(&sem, &val);
    return NULL;
}

void sequential_solution() {
    unsigned long long int inside = 0;
    unsigned long long int all = 0;

    for (unsigned long long int i = 0; i < NUM_POINTS; i++) {
        unsigned int x_k, y_k, z_k;
        double x, y, z;
        x = ((float) rand_r(&x_k) / RAND_MAX) * pi;
        y = (float) rand_r(&y_k) / RAND_MAX;
        z = ((float) rand_r(&z_k) / RAND_MAX) * h;
        if (y <= sin(x) && z <= x * y)
            inside++;
        all++;
    }
    double ratio = (double) inside / (double) all;
    printf("Number of inside points: %lld\nNumber of all points: %lld\nThe ratio: %f\n", inside, all, ratio);
    printf("The result of this integral using sequential solution: %f\n\n", ratio * h * pi);
}

int main(int argc, char *argv[]) {
    struct timespec begin, end;
    double sequential_time, pthread_time;

    clock_gettime(CLOCK_REALTIME, &begin);
    //Sequential solution:
    sequential_solution();
    clock_gettime(CLOCK_REALTIME, &end);
    sequential_time = end.tv_sec - begin.tv_sec;


    clock_gettime(CLOCK_REALTIME, &begin);
    //Pthreads solutions:
    sem_init(&sem, 0, 1);
    pthread_t pthr[NUM_THREADS];
    unsigned long long int param[NUM_THREADS];
    unsigned long long int amount = NUM_POINTS / NUM_THREADS;
    unsigned long long int remainder = NUM_POINTS % NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; ++i) {
        if (i < remainder)
            param[i] = amount + 1;
        else
            param[i] = amount;
        pthread_create(&pthr[i], NULL, func, (void *) &param[i]);
    }
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(pthr[i], NULL);
    }
    double ratio = (double) num_inside_points / (double) num_all_points;
    printf("Number of inside points: %lld\nNumber of all points: %lld\nThe ratio: %f\n", num_inside_points,
           num_all_points, ratio);
    printf("The result of this integral using pthreads: %f\n", ratio * h * pi);
    sem_destroy(&sem);
    clock_gettime(CLOCK_REALTIME, &end);
    pthread_time = end.tv_sec - begin.tv_sec;

    printf("Speed up %f\n", sequential_time / pthread_time);

    return 0;
}

