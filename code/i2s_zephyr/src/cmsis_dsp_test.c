#include "cmsis_dsp_test.h"

#include <stdio.h>

#include "arm_math.h"
#include "arm_const_structs.h"

float32_t testInput[32] =
{
    0.0,
    1.9509032201612824,
    3.826834323650898,
    5.555702330196022,
    7.071067811865475,
    8.314696123025453,
    9.238795325112868,
    9.807852804032304,
    10.0,
    9.807852804032304,
    9.238795325112868,
    8.314696123025454,
    7.0710678118654755,
    5.555702330196022,
    3.826834323650899,
    1.9509032201612861,
    1.2246467991473533e-15,
    -1.9509032201612837,
    -3.8268343236508966,
    -5.55570233019602,
    -7.071067811865475,
    -8.314696123025453,
    -9.238795325112864,
    -9.807852804032303,
    -10.0,
    -9.807852804032304,
    -9.238795325112866,
    -8.314696123025454,
    -7.071067811865477,
    -5.555702330196022,
    -3.826834323650904,
    -1.9509032201612873
};
float32_t testOutput[32];

void cmsis_dsp_test()
{ 
    arm_rfft_fast_instance_f32 S; // RFFT instance
    arm_status status;
    status = arm_rfft_fast_init_f32(&S, 32);
    arm_rfft_fast_f32(&S, testInput, testOutput, 0);

    if ( status != ARM_MATH_SUCCESS)
    {
        while (1);
    }
    printf("N, Amplitude\r\n");
    for (int i = 0; i < sizeof(testInput)/sizeof(testInput[0]); i++)
    {
        printf("%d, %f\r\n", i, testInput[i]);
    }

    printf("N, Freq\r\n");
    for (int i = 0; i < sizeof(testOutput)/sizeof(testOutput[0]); i++)
    {
        printf("%d, %f\r\n", i, testOutput[i]);
    }
}