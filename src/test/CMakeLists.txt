cmake_minimum_required(VERSION 2.8)

project (faster-tester)

set (faster_VERSION_MAJOR 0)
set (faster_VERSION_MINOR 1)

add_definitions(-O3)
add_definitions(-g3)
add_definitions(-ggdb)
#add_definitions(-Wall -Wextra)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ")

find_package(OpenMP)
if(OPENMP_FOUND)
 set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
 set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif(OPENMP_FOUND)

find_package(MPI REQUIRED)
#set (MPI_CXX_COMPILER "mpicc")
add_definitions(${MPI_CXX_COMPILE_FLAGS})
include_directories(${MPI_CXX_INCLUDE_PATH})
link_directories(${MPI_CXX_LIBRARIES})
#set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
#set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})
#include_directories("/usr/include/openmpi/")
set (EXTRA_LIBS ${EXTRA_LIBS} ${MPI_CXX_LIBRARIES})

include_directories ("${PROJECT_SOURCE_DIR}/../libfaster")
set (EXTRA_LIBS ${EXTRA_LIBS} faster)

#set (EXTRA_LIBS ${EXTRA_LIBS} efence)

add_executable(ftest ftest.cpp)
add_executable(latency latency.cpp)

target_link_libraries (ftest  ${EXTRA_LIBS})
target_link_libraries (latency  ${EXTRA_LIBS})

