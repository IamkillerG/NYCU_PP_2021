#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    MPI_Win win;

    // TODO: MPI init
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Status status;

    unsigned int seed = time(NULL);
    srand(seed * world_rank);
    long long int sum = 0;
    long long int n = tosses / world_size;
    long long int number_in_circle = 0;

    double x, y;

    if (world_rank == 0)
    {
        // Master
        MPI_Win_create(&sum, sizeof(long long int), 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    }
    else
    {
        // Workers
        MPI_Win_create(MPI_BOTTOM, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    }

    for(int i=0; i < n ; i++)
    {
        x = rand_r(&seed) / ((double)RAND_MAX);
        y = rand_r(&seed) / ((double)RAND_MAX);

        double distance_squared = x * x + y * y;
        if (distance_squared <= 1)
        {
            number_in_circle++;
        } 
    }

    MPI_Win_fence(0, win);
    MPI_Accumulate(&number_in_circle, 1, MPI_LONG_LONG, 0, 0, 1, MPI_LONG_LONG, MPI_SUM, win);
    MPI_Win_fence(0, win);

    MPI_Win_free(&win);

    if (world_rank == 0)
    {
        // TODO: handle PI result
        pi_result = (double)sum / (double)(n * world_size) * 4.0;
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }
    
    MPI_Finalize();
    return 0;
}