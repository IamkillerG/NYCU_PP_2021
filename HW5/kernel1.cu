#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>

#define BLOCK_SIZE 16

__device__ int mandel(float c_re, float c_im, int count)
{
    float z_re = c_re, z_im = c_im;
    int i;
    for (i = 0; i < count; ++i)
    {
        if (z_re * z_re + z_im * z_im > 4.f)
        break;

        float new_re = z_re * z_re - z_im * z_im;
        float new_im = 2.f * z_re * z_im;
        z_re = c_re + new_re;
        z_im = c_im + new_im;
    }

    return i;
}


__global__ void mandelKernel(float lowerX, float lowerY, float resX, float resY, float stepX, float stepY, int *output, int maxIterations) {
    // To avoid error caused by the floating number, use the following pseudo code
    //
    // float x = lowerX + thisX * stepX;
    // float y = lowerY + thisY * stepY;
    int thisX = blockIdx.x * blockDim.x + threadIdx.x;
    int thisY = blockIdx.y * blockDim.y + threadIdx.y;

    if(thisX >= resX || thisY >= resY) return;

    float x = lowerX + thisX * stepX;
    float y = lowerY + thisY * stepY;

    int index = thisY * resX + thisX;
    output[index] = mandel(x, y, maxIterations);


}

// Host front-end function that allocates the memory and launches the GPU kernel
void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations)
{
    float stepX = (upperX - lowerX) / resX;
    float stepY = (upperY - lowerY) / resY;
    int N = resX * resY;
    int size = N * sizeof(int);

    int *data;
    data = (int*) malloc(size);
    int *d_data;
    cudaMalloc(&d_data, size);

    dim3 blockSize(BLOCK_SIZE, BLOCK_SIZE);
    dim3 numBlocks(resX / blockSize.x, resY / blockSize.y);
    mandelKernel<<<numBlocks, blockSize>>>(lowerX, lowerY, resX, resY, stepX, stepY, d_data, maxIterations);

    cudaMemcpy(data, d_data, size, cudaMemcpyDeviceToHost);
    memcpy(img, data, size);
    cudaFree(d_data);
    free(data);
}