cmake_minimum_required(VERSION 2.8)
project(Mobile_Accelerometer)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_BUILD_TYPE "Release")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(THIRD_PARTY_LIBS wiringPi wiringPiDev pthread m crypt rt)

include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/include)

add_library(ADS1115 SHARED src/ads1115.cpp)
add_executable(ads1115_test src/ads1115_test.cpp)
target_link_libraries(ads1115_test ADS1115 ${THIRD_PARTY_LIBS})
