cmake_minimum_required(VERSION 2.8)
project(eye-detect)
find_package(OpenCV REQUIRED)
#add_executable(eye-detect src/main.cpp src/NeuralNetwork.cpp src/NeuralNetwork.h src/Reader.h src/Reader.cpp src/helpers.h src/helpers.cpp src/constants.h)
add_executable(eye-detect src/test.cpp)
target_link_libraries(eye-detect ${OpenCV_LIBS})
