#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

using namespace std;

typedef struct
{
    int thread_id;
    int start;
    int end;
    long long int *sum;
} Arg;



pthread_mutex_t mutexSum;

void *MonteCarlo(void *arg)
{
    Arg *data = (Arg *) arg;
    int thread_id = data -> thread_id;
    int start = data -> start;
    int end = data -> end;

    long long int *sum = data -> sum;
    long long int number_in_circle = 0;
    unsigned int seed = rand();

    for (int i = start ; i < end ; i++)
    {
        double x = 2.0 * rand_r(&seed) / (RAND_MAX + 1.0) - 1.0;
        double y = 2.0 * rand_r(&seed) / (RAND_MAX + 1.0) - 1.0;
        double distance_squared = x * x + y * y;
        if (distance_squared <=1)
        {
            number_in_circle ++;
        }
    }

    //  critical section   
    pthread_mutex_lock(&mutexSum);
    *sum += number_in_circle;
    pthread_mutex_unlock(&mutexSum);

    pthread_exit((void*)0); 


}




int main(int argc, char *argv[])
{
    srand(time(NULL));

    if(argc != 3)
    {
        cout << "Please enter correct format. Ex, ./pi.out 2 1000000" << endl;
        return 1;
    }

    int threadCount = atoi(argv[1]);
    long long int tossesCount = atoll(argv[2]);
    int step = tossesCount / threadCount;
    Arg arg[threadCount];

    long long int *sum = (long long int*)malloc(sizeof(*sum));
    *sum = 0;

    pthread_t *thread;
    thread = (pthread_t*)malloc(threadCount *sizeof(pthread_t));

    // initial mutex lock
    pthread_mutex_init(&mutexSum,NULL);


    for (int i=0; i < threadCount; i++)
    {
        arg[i].thread_id = i;
        arg[i].start = step * i;
        arg[i].end = step * (i+1);
        arg[i].sum = sum;

        pthread_create(&thread[i],NULL,MonteCarlo,(void*)&arg[i]);
    }


    for(int i = 0; i < threadCount; i++)
    {
        pthread_join(thread[i],NULL);
    }

    pthread_mutex_destroy(&mutexSum);

    double pi = 4.0 * (*sum) / (double)tossesCount;

    cout.precision(6);
    cout << fixed << pi << endl;
    
    return 0;

}


