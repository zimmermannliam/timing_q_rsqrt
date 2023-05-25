#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <immintrin.h>

#define NUMT 100000
#define MAX_DIFF 5.0
#define RANDO_MAX 5
#define RANDO_MIN 0

float Q_rsqrt( float number );

int main()
{
    srand(time(NULL));

    float normal[NUMT];
    float q_rsqrt[NUMT];
    float rsqrtss[NUMT];

    float rando[NUMT];

    double start, end;

    __m128 temp;

    size_t i;
    
    /* Setup a random list */
    for (i = 0; i < NUMT; ++i)
    {
        rando[i] = (RANDO_MAX - RANDO_MIN) * ((float) rand() / RAND_MAX) + RANDO_MIN;
    }

    /* Normal trial */
    start = omp_get_wtime();
    for (i = 0; i < NUMT; ++i)
    {
        normal[i] = 1 / sqrt(rando[i]);
    }
    end = omp_get_wtime();
    printf("normal: %10.2lf microseconds\n", 1000000. * (end - start));
    
    /* Q_rsqrt trial */
    start = omp_get_wtime();
    for (i = 0; i < NUMT; ++i)
    {
        q_rsqrt[i] = Q_rsqrt(rando[i]);
    }
    end = omp_get_wtime();
    printf("Q_rsqrt: %10.2lf microseconds\n", 1000000. * (end - start));

    /* rsqrtss trial */
    start = omp_get_wtime();
    for (i = 0; i < NUMT; ++i)
    {
        temp = _mm_set_ss(rando[i]);
        rsqrtss[i] = _mm_cvtss_f32(_mm_rsqrt_ss(temp));
    }
    end = omp_get_wtime();
    printf("rsqrtss : %10.2lf microseconds\n", 1000000. * (end - start));

    /* Get the maximum error for each */
    /* Is it really error..? */
    float q_rsqrt_max_error = 0.;
    float rsqrtss_max_error = 0.;
    for (i = 0; i < NUMT; ++i)
    {
        if (q_rsqrt_max_error < fabs(normal[i] - q_rsqrt[i]))
            q_rsqrt_max_error = fabs(normal[i] - q_rsqrt[i]);
        if (rsqrtss_max_error < fabs(normal[i] - rsqrtss[i]))
            rsqrtss_max_error = fabs(normal[i] - rsqrtss[i]);
    }
    printf("Q_rsqrt max error: %f\nrsqrtss max error: %f\n", q_rsqrt_max_error, rsqrtss_max_error);
}


float Q_rsqrt( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       
	i  = 0x5f3759df - ( i >> 1 );              
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) ); 

	return y;
}
