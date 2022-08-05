# include <stdio.h>
# include <stdlib.h>
# include <mpi.h>

# define MASTER 0
# define FROM_MASTER 1
# define FROM_WORKER 2

// Read size of matrix_a and matrix_b (n, m, l) and whole data of matrixes from stdin
//
// n_ptr:     pointer to n
// m_ptr:     pointer to m
// l_ptr:     pointer to l
// a_mat_ptr: pointer to matrix a (a should be a continuous memory space for placing n * m elements of int)
// b_mat_ptr: pointer to matrix b (b should be a continuous memory space for placing m * l elements of int)
void construct_matrices(int *n_ptr, int *m_ptr, int *l_ptr,
	         	int **a_mat_ptr, int **b_mat_ptr){
    int size, rank;
    int *ptr;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == MASTER)
    {
        scanf("%d %d %d", n_ptr, m_ptr, l_ptr);
        int n = *n_ptr;
        int m = *m_ptr;
        int l = *l_ptr;
        *a_mat_ptr = (int*)malloc(sizeof(int) * n * m);
        *b_mat_ptr = (int*)malloc(sizeof(int) * m * l);

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                ptr = *a_mat_ptr + i * m + j;
                scanf("%d", ptr);
            }
        }

        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < l; j++)
            {
                ptr = *b_mat_ptr + i * l + j;
                scanf("%d", ptr);
            }
        }
    }
}

// Just matrix multiplication (your should output the result in this function)
//
// n:     row number of matrix a
// m:     col number of matrix a / row number of matrix b
// l:     col number of matrix b
// a_mat: a continuous memory placing n * m elements of int
// b_mat: a continuous memory placing m * l elements of int
void matrix_multiply(const int n, const int m, const int l,
                     const int *a_mat, const int *b_mat){
    int size, rank;
    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int	numtasks,              /* number of tasks in partition */
	    taskid,                /* a task identifier */
	    numworkers,            /* number of worker tasks */
	    source,                /* task id of message source */
	    dest,                  /* task id of message destination */
	    mtype,                 /* message type */
	    rows,                  /* rows of matrix A sent to each worker */
	    averow, extra, offset; /* used to determine rows sent to each worker */

    int N, M, L;
    numworkers = size - 1;

    // master task
    if (rank == MASTER)
    {
        int *ans;
    	ans = (int*)malloc(sizeof(int) * n * l);
        // Send matrix data to the worker tasks 
        averow = n / numworkers;
        extra = n % numworkers;
        offset = 0;
        mtype = FROM_MASTER;
        for (dest = 1; dest <= numworkers; dest++)
        {
            MPI_Send(&n, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(&m, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(&l, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            rows = (dest <= extra)? averow + 1: averow;
            MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(&a_mat[offset * m], rows * m, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(&b_mat[0], m * l, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            offset += rows;
        }

        // Receive results from worker tasks 
        mtype = FROM_WORKER;
        for (int i = 1; i <= numworkers; i++)
        {
            source = i;
            MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&ans[offset * l], rows * l, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
        }

        // Print results 
        for (int i=0; i < n; i++)
        {
            for (int j = 0; j < l; j++)
            {
                printf("%d", ans[i * l + j]);
                if(j != l-1) printf(" ");
            }
            printf("\n");
        }
	    free(ans);
    }

    // worker task
    if (rank > MASTER)
    {
        mtype = FROM_MASTER;
        MPI_Recv(&N, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&M, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&L, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
	    int *a = (int*)malloc(sizeof(int) * N * M);;
    	int *b = (int*)malloc(sizeof(int) * M * L);
    	int *c = (int*)malloc(sizeof(int) * N * L);

        MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&a[0], rows * M, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&b[0], M * L, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);

        for (int k = 0; k < L; k++)
        {
            for (int i = 0; i < rows; i++)
            {
                c[i * L + k] = 0;
                for (int j = 0; j < M; j++)
                {
                    c[i * L + k] += a[i * M + j] * b[j * L + k];
                }
            }
        }

        mtype = FROM_WORKER;
        MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
        MPI_Send(&c[0], rows * L, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);

	    free(a);
        free(b);
	    free(c);
    }
}

// Remember to release your allocated memory
void destruct_matrices(int *a_mat, int *b_mat){
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == MASTER){
	free(a_mat);
	free(b_mat);
    }
}