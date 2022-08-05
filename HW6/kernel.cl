__kernel void convolution(int filterWidth,
                         __constant float *filter, 
                         int imageHeight,
                         int imageWidth,
                         const __global float *inputImage, 
                         __global float *outputImage) 
{
    int cl_width = get_global_id(0);
    int cl_height = get_global_id(1);
    float sum;
    int index = cl_height * imageWidth + cl_width ;
    int halffilterSize = filterWidth / 2;
   
    sum = 0.0;
    if (cl_width >= imageWidth || cl_height >= imageHeight)
    {
        return; 
    }
    for(int k = -halffilterSize; k <= halffilterSize; k++)
    {
        for(int l = -halffilterSize; l <= halffilterSize; l++)
        {
            if(cl_height + k >= 0 && cl_height + k < imageHeight &&
                cl_width + l >= 0 && cl_width + l < imageWidth)
            {
                sum += inputImage[(cl_height + k) * imageWidth + cl_width + l] *
                       filter[(k + halffilterSize) * filterWidth + 
                              l + halffilterSize];
            }
        }
    }
    outputImage[index] = sum;
}