cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr)
project(my_zephyr_app)

set(CMSIS_DSP_INCLUDE_DIR "/home/mahesh/CMSIS/CMSIS_5-5.7.0/CMSIS/DSP/Include")
set(CMSIS_DSP_SRC_DIR "/home/mahesh/CMSIS/CMSIS_5-5.7.0/CMSIS/DSP/Source")

set(CMSIS_NN_INCLUDE_DIR "/home/mahesh/CMSIS/CMSIS_5-5.7.0/CMSIS/NN/Include")
set(CMSIS_NN_SRC_DIR "/home/mahesh/CMSIS/CMSIS_5-5.7.0/CMSIS/NN/Source")

target_include_directories(app PRIVATE ${CMSIS_DSP_INCLUDE_DIR})
target_include_directories(app PRIVATE ${CMSIS_NN_INCLUDE_DIR})

# Add your source file to the "app" target. This must come after
# find_package(Zephyr) which defines the target.
target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/loopback.c)
target_sources(app PRIVATE src/i2s_mic.c)
target_sources(app PRIVATE src/cmsis_dsp_test.c)
target_sources(app PRIVATE src/cmsis_nn_test.c)

target_sources(app PRIVATE ${CMSIS_DSP_SRC_DIR}/TransformFunctions/TransformFunctions.c)
#target_sources(app PRIVATE ${CMSIS_DSP_SRC_DIR}/CommonTables/arm_const_structs.c)
#target_sources(app PRIVATE ${CMSIS_DSP_SRC_DIR}/CommonTables/arm_common_tables.c)

target_sources(app PRIVATE ${CMSIS_NN_SRC_DIR}/SoftmaxFunctions/arm_softmax_q7.c)
target_sources(app PRIVATE ${CMSIS_NN_SRC_DIR}/ActivationFunctions/arm_nn_activations_q7.c)
target_sources(app PRIVATE ${CMSIS_NN_SRC_DIR}/ActivationFunctions/arm_relu_q7.c)
target_sources(app PRIVATE ${CMSIS_NN_SRC_DIR}/ConvolutionFunctions/arm_convolve_HWC_q7_fast.c)
target_sources(app PRIVATE ${CMSIS_NN_SRC_DIR}/ConvolutionFunctions/arm_convolve_HWC_q7_RGB.c)
target_sources(app PRIVATE ${CMSIS_NN_SRC_DIR}/FullyConnectedFunctions/arm_fully_connected_q7.c)
target_sources(app PRIVATE ${CMSIS_NN_SRC_DIR}/FullyConnectedFunctions/arm_fully_connected_q7_opt.c)
target_sources(app PRIVATE ${CMSIS_NN_SRC_DIR}/PoolingFunctions/arm_pool_q7_HWC.c)
target_sources(app PRIVATE ${CMSIS_NN_SRC_DIR}/NNSupportFunctions/arm_q7_to_q15_reordered_no_shift.c)
target_sources(app PRIVATE ${CMSIS_NN_SRC_DIR}/ConvolutionFunctions/arm_nn_mat_mult_kernel_q7_q15_reordered.c)
target_sources(app PRIVATE ${CMSIS_NN_SRC_DIR}/ConvolutionFunctions/arm_nn_mat_mult_kernel_q7_q15.c)
