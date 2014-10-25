#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef union {
  int32_t i;
  float f;
} poly32_t;

float log2_approx_first_order(float x)
{
    poly32_t poly_x;
    poly_x.f = x;
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

#define nbits 8
float log2_approx_second_order(float x)
{
    static float table[] = {
0.00000000,0.00562455,0.01122726,0.01680829,0.02236781,0.02790600,0.03342300,0.03891899,
0.04439412,0.04984855,0.05528244,0.06069593,0.06608919,0.07146236,0.07681560,0.08214904,
0.08746284,0.09275714,0.09803208,0.10328781,0.10852446,0.11374217,0.11894108,0.12412131,
0.12928301,0.13442633,0.13955136,0.14465824,0.14974712,0.15481810,0.15987134,0.16490693,
0.16992500,0.17492568,0.17990910,0.18487534,0.18982457,0.19475685,0.19967234,0.20457114,
0.20945336,0.21431912,0.21916851,0.22400168,0.22881868,0.23361968,0.23840474,0.24317399,
0.24792752,0.25266543,0.25738785,0.26209486,0.26678655,0.27146304,0.27612442,0.28077078,
0.28540221,0.29001886,0.29462075,0.29920802,0.30378073,0.30833903,0.31288296,0.31741261,
0.32192808,0.32642949,0.33091688,0.33539036,0.33985001,0.34429592,0.34872815,0.35314682,
0.35755199,0.36194378,0.36632222,0.37068740,0.37503943,0.37937838,0.38370430,0.38801730,
0.39231741,0.39660478,0.40087944,0.40514147,0.40939093,0.41362792,0.41785252,0.42206475,
0.42626476,0.43045256,0.43462822,0.43879184,0.44294348,0.44708323,0.45121112,0.45532721,
0.45943162,0.46352437,0.46760556,0.47167522,0.47573343,0.47978026,0.48381579,0.48784003,
0.49185309,0.49585503,0.49984589,0.50382572,0.50779462,0.51175267,0.51569986,0.51963627,
0.52356195,0.52747703,0.53138149,0.53527540,0.53915882,0.54303181,0.54689443,0.55074680,
0.55458885,0.55842072,0.56224245,0.56605405,0.56985563,0.57364720,0.57742882,0.58120060,
0.58496249,0.58871466,0.59245706,0.59618974,0.59991282,0.60362637,0.60733032,0.61102480,
0.61470985,0.61838549,0.62205184,0.62570882,0.62935662,0.63299519,0.63662463,0.64024496,
0.64385617,0.64745843,0.65105170,0.65463603,0.65821147,0.66177809,0.66533589,0.66888499,
0.67242533,0.67595702,0.67948008,0.68299460,0.68650055,0.68999797,0.69348693,0.69696754,
0.70043969,0.70390356,0.70735914,0.71080643,0.71424550,0.71767640,0.72109920,0.72451383,
0.72792047,0.73131901,0.73470962,0.73809224,0.74146700,0.74483383,0.74819285,0.75154406,
0.75488752,0.75822324,0.76155126,0.76487160,0.76818430,0.77148944,0.77478707,0.77807713,
0.78135973,0.78463483,0.78790253,0.79116291,0.79441589,0.79766154,0.80089992,0.80413103,
0.80735493,0.81057161,0.81378120,0.81698364,0.82017899,0.82336724,0.82654852,0.82972276,
0.83289003,0.83605033,0.83920377,0.84235036,0.84549004,0.84862292,0.85174906,0.85486841,
0.85798097,0.86108691,0.86418617,0.86727875,0.87036473,0.87344414,0.87651694,0.87958324,
0.88264304,0.88569635,0.88874322,0.89178371,0.89481777,0.89784545,0.90086681,0.90388185,
0.90689057,0.90989310,0.91288936,0.91587937,0.91886324,0.92184097,0.92481250,0.92777795,
0.93073732,0.93369067,0.93663794,0.93957919,0.94251448,0.94544381,0.94836724,0.95128471,
0.95419633,0.95710206,0.96000195,0.96289599,0.96578431,0.96866679,0.97154355,0.97441459,
0.97727990,0.98013955,0.98299360,0.98584193,0.98868471,0.99152184,0.99435341,0.99717951
};

    poly32_t poly_x;
    poly_x.f = x;

    poly32_t poly_x_exponent;
    poly32_t poly_x_significand;
    
    float y_approx;
    poly_x_exponent.i = (0xff & (poly_x.i >> 23) ) - 127;
    if(poly_x_exponent.i != -127) {
        // extract the exponent and significand
        poly_x_significand.i = (0x007fffff & poly_x.i);
        int sig_lookup = poly_x_significand.i >> (23-nbits);
        
        // log2(2^exp * sig) = exponent + log2(significand)
        float log_sig = table[sig_lookup];
        y_approx = (float)poly_x_exponent.i + log_sig;
    }
    else {
        y_approx = -INFINITY;
    }
    return y_approx;
}

float log2_approx_third_order(float x)
{
    static const float bin_frac_scale = (float)(1<<23);
    poly32_t poly_x;
    poly_x.f = x;

    poly32_t poly_x_exponent;
    poly32_t poly_x_significand;
    
    float y_approx;
    poly_x_exponent.i = (0xff & (poly_x.i >> 23) ) - 127;
    if(poly_x_exponent.i != -127) {
        // extract the exponent and significand
        poly_x_significand.i = (0x007fffff & poly_x.i);
        float x_sig_f = 1.0 + ((float)poly_x_significand.i)/bin_frac_scale;
        float log_sig = -1.64913 + 1.99505*x_sig_f + -0.33691*x_sig_f*x_sig_f;

        // log2(2^exp * sig) = exponent + log2(significand)
        y_approx = (float)poly_x_exponent.i + log_sig;
    }
    else {
        y_approx = -INFINITY;
    }
    return y_approx;
}

void gen_table(int significant_bits)
{
    int max_bits = 23; // 23-bit significand
    int table_size = 1 << significant_bits;
    float table[table_size];
    poly32_t indep_var[table_size];

    unsigned int ii;
    for(ii = 0; ii < table_size; ++ii) {
        indep_var[ii].i = (ii<<(23-significant_bits));
        table[ii] = log2( 1.0 +  (float)(ii<<(23-significant_bits))/(float)(1<<23) );
    }

    printf("the actual log:\n");
    for(ii = 0; ii < table_size; ++ii) {
        printf("%6.8f,", table[ii]);
        if( (1+ii) % 8 == 0 ) {
            printf("\n");
        }
    }
    printf("dependent var:\n");
    for(ii = 0; ii < table_size; ++ii) {
        printf("%i,", indep_var[ii].i);
        if( (1+ii) % 8 == 0 ) {
            printf("\n");
        }
    }
}

int main()
{
    float x_init = 31415926535.8;
    poly32_t one;
    one.f = 1.0f;
    clock_t start, end;
    float math_time, approx_time;

    //gen_table(nbits);

    uint32_t ii;
    const uint32_t num_iter = 100000;
    float y_math[num_iter];
    float y_approx[num_iter];
    float x[num_iter];
    x[0]=x_init;
    for(ii=1; ii < num_iter; ++ii) {
        x[ii] = x[ii-1] * 0.7;
    }

    start = clock();
    for(ii=0; ii < num_iter; ++ii) {
        y_math[ii] = log2(x[ii]);
    }
    end = clock();
    math_time = (float)(end - start) / CLOCKS_PER_SEC;
    
    start = clock();
    for(ii=0; ii < num_iter; ++ii) {
        y_approx[ii] = log2_approx_third_order(x[ii]);
    }
    end = clock();
    approx_time = (float)(end - start) / CLOCKS_PER_SEC;

    printf("[math time: %1.4e]  [approx time: %1.4e]\n", math_time, approx_time);
    for(ii=0; ii < num_iter; ++ii) {
        float diff = (y_approx[ii] - y_math[ii])/y_math[ii];
        if( diff > 1e-3)
            printf("[%i] diff: %1.5f, input: %e, [math] %f, [[aprox] %f\n", ii, diff, x[ii], y_math[ii], y_approx[ii]);
    }

    return 1;
}
