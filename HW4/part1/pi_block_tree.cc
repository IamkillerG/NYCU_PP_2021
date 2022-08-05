#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

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

    MPI_Barrier(MPI_COMM_WORLD);

    // TODO: binary tree redunction
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
    if(world_size > 1)
    {
        for(int i = 1; pow(2,i) <= world_size; i++)
        {
            if(world_rank%(int)(pow(2,i)) == 0)
            {
                sum = number_in_circle;
                MPI_Recv(&number_in_circle, 1, MPI_LONG_LONG, world_rank+pow(2,i-1), 0, MPI_COMM_WORLD, &status);
                sum += number_in_circle;
                number_in_circle = sum;
            }
            else
            {
                MPI_Send(&number_in_circle, 1, MPI_LONG_LONG, world_rank-pow(2,i-1), 0, MPI_COMM_WORLD);
                break;
            }
        }
    }
    else
    {
        sum = number_in_circle;
    }
    
    if (world_rank == 0)
    {
        // TODO: PI result
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
