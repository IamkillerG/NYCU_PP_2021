#include <stdio.h>
#include <stdlib.h>
#include "hostFE.h"
#include "helper.h"

void hostFE(int filterWidth, float *filter, int imageHeight, int imageWidth,
            float *inputImage, float *outputImage, cl_device_id *device,
            cl_context *context, cl_program *program)
{
    cl_int status;
    int filterSize = filterWidth * filterWidth * sizeof(float);
    int imageSize = imageHeight * imageWidth * sizeof(float);

    // create command queue
    cl_command_queue cmd_queue;
    cmd_queue = clCreateCommandQueue(*context, *device, 0, &status);

    // create buffers on device
    cl_mem d_in = clCreateBuffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imageSize, inputImage, &status);
    cl_mem d_filter = clCreateBuffer(*context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, filterSize, filter, &status);
    cl_mem d_out = clCreateBuffer(*context, CL_MEM_WRITE_ONLY, imageSize, NULL, &status);


    // create OpenCL kernel
    cl_kernel clkernel = clCreateKernel(*program, "convolution", &status);

    // set arguments
    clSetKernelArg(clkernel, 0, sizeof(int), (void *)&filterWidth);
    clSetKernelArg(clkernel, 1, sizeof(cl_mem), (void *)&d_filter);
    clSetKernelArg(clkernel, 2, sizeof(int), (void *)&imageHeight);
    clSetKernelArg(clkernel, 3, sizeof(int), (void *)&imageWidth);
    clSetKernelArg(clkernel, 4, sizeof(cl_mem), (void *)&d_in);
    clSetKernelArg(clkernel, 5, sizeof(cl_mem), (void *)&d_out);


    size_t globalws[2] = {imageWidth, imageHeight};
	
    // execute kernel
    clEnqueueNDRangeKernel(cmd_queue, clkernel, 2, 0, &globalws, NULL, 0, NULL, NULL);

    // copy  back to host
    clEnqueueReadBuffer(cmd_queue, d_out, CL_TRUE, 0, imageSize, outputImage, 0, NULL, NULL);

    // free
		
    status = clFlush(cmd_queue);
    status = clFinish(cmd_queue);
    status = clReleaseKernel(clkernel);
    status = clReleaseMemObject(d_in);
    status = clReleaseMemObject(d_filter);  
    status = clReleaseMemObject(d_out);
    status = clReleaseCommandQueue(cmd_queue);
}