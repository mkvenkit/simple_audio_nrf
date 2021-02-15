#include "cmsis_nn_test.h"

#include <arm_math.h>
#include <arm_nnfunctions.h>

void cmsis_nn_test()
{
    q7_t      output_data[8];
    arm_softmax_q7(output_data, 10, output_data);

}
