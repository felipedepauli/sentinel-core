cmake_minimum_required(VERSION 3.10)
project(Eys)

set(CMAKE_CXX_STANDARD 14)
set(OpenCV_DIR /home/fpauli/git/opencv-4.5.4/platforms/linux/build_hardfp) # Adicione esta linha

# Cross compiling configuration
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_BUILD_TYPE Debug)

set(tools /usr)
set(CMAKE_C_COMPILER ${tools}/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${tools}/bin/arm-linux-gnueabihf-g++)

# Compiling flags for Raspberry Pi
add_compile_options(-march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=hard -mtune=cortex-a72)

# Enable testing
enable_testing()

# Find the required packages for the project
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED)
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

# Include the required directories
include_directories(${GTEST_INCLUDE_DIRS})

# Create the executable
add_executable(Eys main.cpp Eys.cpp Session.cpp)

# Link the required libraries
target_link_libraries(Eys ${OpenCV_LIBS} ${Boost_LIBRARIES}  Threads::Threads)

# add_executable(tests your_test_file.cpp)
# target_link_libraries(tests ${GTEST_BOTH_LIBRARIES} pthread)

