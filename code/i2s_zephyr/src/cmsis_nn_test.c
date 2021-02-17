#include "cmsis_nn_test.h"

#include <stdio.h>

#include <arm_math.h>
#include <arm_nnfunctions.h>

#include "spectrogram.h"
#include "weights.h"


#define CONV1_IM_DIM 32
#define CONV1_IM_CH 1
#define CONV1_KER_DIM 3
#define CONV1_PADDING 0
#define CONV1_STRIDE 2
#define CONV1_OUT_CH 16
#define CONV1_OUT_DIM 15

#define POOL1_KER_DIM 2
#define POOL1_STRIDE 0
#define POOL1_PADDING 0
#define POOL1_OUT_DIM 7

#define IP1_DIM 784 // 7*7*16
#define IP1_OUT 4

static q7_t conv1_wt[CONV1_IM_CH * CONV1_KER_DIM * CONV1_KER_DIM * CONV1_OUT_CH] = CONV1_WT;
static q7_t conv1_bias[CONV1_OUT_CH] = CONV1_BIAS;

static q7_t ip1_wt[IP1_DIM * IP1_OUT] = IP1_WT;
static q7_t ip1_bias[IP1_OUT] = IP1_BIAS;

/* Here the image_data should be the raw uint8 type RGB image in [RGB, RGB, RGB ... RGB] format */
uint8_t   image_data[CONV1_IM_CH * CONV1_IM_DIM * CONV1_IM_DIM] = IMG_DATA;
q7_t      output_data[IP1_OUT];

// https://www.dlology.com/blog/how-to-run-deep-learning-model-on-microcontroller-with-cmsis-nn-part-2/
// To find out the required col_buffer size across all convolutional layers, this formula below is applied.
// 2*2*(conv # of filters)*(kernel width)*(kernel height)
// 2*2*16*3*3

//vector buffer: max(im2col buffer,average pool buffer, fully connected buffer)
q7_t      col_buffer[2 * 3 * 3 * 16 * 2];

q7_t      scratch_buffer[15*15*16 + 7*7*16];

void cmsis_nn_test()
{
    q7_t     *img_buffer1 = scratch_buffer;
    q7_t     *img_buffer2 = img_buffer1 + 15*15*16;

    arm_status status = ARM_MATH_SUCCESS;
    // convolution layer
    arm_convolve_HWC_q7_RGB(img_buffer1, CONV1_IM_DIM, CONV1_IM_CH, conv1_wt, CONV1_OUT_CH, CONV1_KER_DIM, CONV1_PADDING,
                            CONV1_STRIDE, conv1_bias, CONV1_BIAS_LSHIFT, CONV1_OUT_RSHIFT, img_buffer1, CONV1_OUT_DIM,
                            (q15_t *) col_buffer, NULL);

    /*
arm_status arm_convolve_HWC_q7_basic	(	
    const q7_t * 	Im_in,
const uint16_t 	dim_im_in,
const uint16_t 	ch_im_in,
const q7_t * 	wt,
const uint16_t 	ch_im_out,
const uint16_t 	dim_kernel,
const uint16_t 	padding,
const uint16_t 	stride,
const q7_t * 	bias,
const uint16_t 	bias_shift,
const uint16_t 	out_shift,
q7_t * 	Im_out,
const uint16_t 	dim_im_out,
q15_t * 	bufferA,
q7_t * 	bufferB 
)	*/
    status = arm_convolve_HWC_q7_basic(image_data, CONV1_IM_DIM, CONV1_IM_CH, conv1_wt, CONV1_OUT_CH, CONV1_KER_DIM, CONV1_PADDING,
                            CONV1_STRIDE, conv1_bias, CONV1_BIAS_LSHIFT, CONV1_OUT_RSHIFT, img_buffer1, CONV1_OUT_DIM,
                            (q15_t *) col_buffer, NULL);

    if (status != ARM_MATH_SUCCESS) {
        printf("conv2d failed with status = %d\n", status);
    }

    // relu 
    arm_relu_q7(img_buffer1, CONV1_OUT_DIM * CONV1_OUT_DIM * CONV1_OUT_CH);

    if (status != ARM_MATH_SUCCESS) {
        printf("relu failed with status = %d\n", status);
    }

    // maxpool
    arm_maxpool_q7_HWC(img_buffer1, CONV1_OUT_DIM, CONV1_OUT_CH, POOL1_KER_DIM,
                        POOL1_PADDING, POOL1_STRIDE, POOL1_OUT_DIM, NULL, img_buffer2);

    if (status != ARM_MATH_SUCCESS) {
        printf("maxpool failed with status = %d\n", status);
    }

    // dense layer 
    arm_fully_connected_q7_opt(img_buffer2, ip1_wt, IP1_DIM, IP1_OUT, IP1_BIAS_LSHIFT, IP1_OUT_RSHIFT, ip1_bias,
                                output_data, (q15_t *) img_buffer1);
    if (status != ARM_MATH_SUCCESS) {
        printf("dense failed with status = %d\n", status);
    }

    printf("dense:\n");
    for (int i = 0; i < 4; i++)
    {
        printf("%d: %2x\n", i, (unsigned char)output_data[i]);
    }
    
    // softmax
    arm_softmax_q7(output_data, 4, output_data);
    if (status != ARM_MATH_SUCCESS) {
        printf("softmax failed with status = %d\n", status);
    }

    printf("softmax:\n");
    for (int i = 0; i < 4; i++)
    {
        printf("%d: %d\n", i, (unsigned char)output_data[i]);
    }
}
