cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr)
project(my_zephyr_app)

set(CMSIS_DSP_INCLUDE_DIR "/home/mahesh/CMSIS/CMSIS_5-5.7.0/CMSIS/DSP/Include")
set(CMSIS_DSP_SRC_DIR "/home/mahesh/CMSIS/CMSIS_5-5.7.0/CMSIS/DSP/Source")

target_include_directories(app PRIVATE ${CMSIS_DSP_INCLUDE_DIR})

# Add your source file to the "app" target. This must come after
# find_package(Zephyr) which defines the target.
target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/loopback.c)
#target_sources(app PRIVATE src/i2s_mic.c)
target_sources(app PRIVATE src/cmsis_dsp_test.c)

target_sources(app PRIVATE ${CMSIS_DSP_SRC_DIR}/TransformFunctions/TransformFunctions.c)
target_sources(app PRIVATE ${CMSIS_DSP_SRC_DIR}/CommonTables/arm_const_structs.c)
target_sources(app PRIVATE ${CMSIS_DSP_SRC_DIR}/CommonTables/arm_common_tables.c)
