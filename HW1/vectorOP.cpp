#include "PPintrin.h"

// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {

    // All ones
    maskAll = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    _pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
  }
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
  //
  // PP STUDENTS TODO: Implement your vectorized version of
  // clampedExpSerial() here.
  //
  // Your solution should work for any value of
  // N and VECTOR_WIDTH, not just when VECTOR_WIDTH divides N
  //

  __pp_vec_float x, result;
  __pp_vec_int y, count ;
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative, maskForWhile, maskForWhileIf;

  __pp_vec_int zero = _pp_vset_int(0);
  __pp_vec_int one = _pp_vset_int(1);
  __pp_vec_float nine = _pp_vset_float(9.999999f);

  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    if(i + VECTOR_WIDTH > N)
    {
      maskAll = _pp_init_ones(N - i);
    }
		else
    {
      maskAll = _pp_init_ones();
    }
  

    maskIsNegative = _pp_init_ones(0);
    maskForWhile = _pp_init_ones(0);
    maskForWhileIf = _pp_init_ones(0);

    _pp_vload_float(x, values + i, maskAll);  // float x = values[i];
    _pp_vload_int(y, exponents + i, maskAll);    // int y = exponents[i];
    
    _pp_veq_int(maskIsNegative, y , zero, maskAll); // if(y == 0)
    _pp_vset_float(result,1.f,maskIsNegative);  // output[i] = 1.f;

    maskIsNotNegative = _pp_mask_not(maskIsNegative); // else{
    
    _pp_vload_float(result, values + i, maskIsNotNegative); // float result = x;
    _pp_vsub_int(count,y, one, maskIsNotNegative); // int count = y-1;
    
    _pp_vgt_int(maskForWhile,count,zero,maskIsNotNegative); // count > 0;

    while (_pp_cntbits(maskForWhile) !=0)
    {
      _pp_vmult_float(result, result, x, maskForWhile); // result  *= x;
      _pp_vsub_int(count,count,one,maskForWhile); // count--;
      _pp_vgt_int(maskForWhile, count, zero, maskForWhile); //count > 0;
    }


    _pp_vgt_float(maskForWhileIf,result, nine,maskIsNotNegative); // if result > 9.99999f
    _pp_vset_float(result,9.999999f,maskForWhileIf); // result = 9.999999f;

    _pp_vstore_float(output + i, result , maskAll); // output[i] = result;

  }
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{

  //
  // PP STUDENTS TODO: Implement your vectorized version of arraySumSerial here
  //

  float sum = 0.0;
  __pp_mask maskAll = _pp_init_ones();
  __pp_vec_float VectorSum , hadd, interleave;


  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    _pp_vload_float(VectorSum, values + i, maskAll);
    _pp_hadd_float(hadd, VectorSum); // add up vector
    _pp_interleave_float(interleave, hadd); // sort them

    for(int j = 0; j < VECTOR_WIDTH / 2 ; j++)  // after interleave only need to addup to width/2
    {
      sum += interleave.value[j];
    }
  }
  

  return sum;
}

