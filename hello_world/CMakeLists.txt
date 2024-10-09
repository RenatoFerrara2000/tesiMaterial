cmake_minimum_required(VERSION 3.20.0)

# Find Zephyr and set the project name
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(tensorflow_hello_world)

# Disable thread-safe initialization of local statics (Zephyr-specific)
set(NO_THREADSAFE_STATICS $<TARGET_PROPERTY:compiler-cpp,no_threadsafe_statics>)
zephyr_compile_options($<$<COMPILE_LANGUAGE:CXX>:${NO_THREADSAFE_STATICS}>)

# Include the TensorFlow Lite Micro library
# ${ZEPHYR_TFLM_MODULE_DIR} is the path to TensorFlow Lite Micro module in Zephyr
zephyr_include_directories(${ZEPHYR_TFLM_MODULE_DIR})

# Specify the source files for your application
file(GLOB app_sources src/*)
target_sources(app PRIVATE ${app_sources})

# Link TensorFlow Lite Micro library to your application
target_link_libraries(app PRIVATE tensorflow_lite_micro)
