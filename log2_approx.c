#include <math.h>
#include <stdint.h>
#include <stdio.h>

typedef union {
  int32_t i;
  float f;
} poly32_t;

float log2_approx(poly32_t poly_x)
{
    poly32_t poly_x_exponent;
    poly32_t poly_x_significand;
    
    // extract the exponent and significand
    poly_x_significand.i = (0x007fffff & poly_x.i);
    poly_x_exponent.i = (0xff & (poly_x.i >> 23) ) - 127;
    
    // recover the hidden 1 of significand
    poly_x_significand.f = 1.0 + ((float)poly_x_significand.i)/(float)(1 << 23);
    // log2(2^exp * sig) = exponent + log2(significand)
    float log_sig = log2(poly_x_significand.f);
    float y_approx = (float)poly_x_exponent.i + log_sig;
    return y_approx;
}



int main()
{
    float x = 31415926535.8;
    poly32_t one;
    one.f = 1.0f;

    uint32_t ii;
    uint32_t num_iter = 15;
    for(ii=0; ii < num_iter; ++ii) {
        poly32_t poly_x;
        poly_x.f = x;
        // Get the actual value
        float y_math = log2(x);
        float y_approx = log2_approx(poly_x);
        printf("math::log2(%16.4f)=%8.4f  ;  approx=%8.4f  ;  diff=%.4f\n",
                                x, y_math,      y_approx, y_math-y_approx);
        x *= 0.1;
    }
    return 1;
}
