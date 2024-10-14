cmake_minimum_required(VERSION 3.20)

# Set the project name
project(hello_world_python LANGUAGES CXX)

# Find Python
find_package(Python REQUIRED COMPONENTS Development)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(tensorflow_hello_world)

# Set Python executable
set(PYTHON_EXECUTABLE ${Python_EXECUTABLE})


add_custom_target(run_jupyter_notebook
    COMMAND ${Python_EXECUTABLE} -m jupyter nbconvert --to notebook --execute --inplace train_hello_world_model.ipynb
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS train_hello_world_model.ipynb
)


# Optionally, add any Python dependencies if needed
# For example, if you have a requirements.txt
# execute_process(COMMAND ${PYTHON_EXECUTABLE} -m pip install -r requirements.txt)

# You can add more targets or options as necessary