cmake_minimum_required(VERSION 2.8)

project (faster-server)

set (faster_VERSION_MAJOR 0)
set (faster_VERSION_MINOR 1)

add_definitions(-O3)
add_definitions(-ggdb)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")

find_package(OpenMP)
if(OPENMP_FOUND)
 set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
 set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif(OPENMP_FOUND)

#set (EXTRA_LIBS ${EXTRA_LIBS} libfaster)

add_executable(fserver fserver.cpp)

target_link_libraries (fserver  ${EXTRA_LIBS})
