cmake_minimum_required(VERSION 3.16.3)
project(g2o)

# default built type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Detect OS and define macros appropriately
if(UNIX)
  add_definitions(-DUNIX)
  message(STATUS "Compiling on Unix")
endif(UNIX)

# Eigen library parallelise itself, though, presumably due to performance issues
# OPENMP is experimental. We experienced some slowdown with it
find_package(OpenMP)
set(G2O_USE_OPENMP
    OFF
    CACHE BOOL "Build g2o with OpenMP support (EXPERIMENTAL)")

# Find Eigen3
find_package(Eigen3 3.1.0 REQUIRED)

# Generate config.h
configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

# Include the subdirectories
add_library(
  ${PROJECT_NAME} SHARED
  # types
  g2o/types/types_sba.cpp
  g2o/types/types_six_dof_expmap.cpp
  g2o/types/types_seven_dof_expmap.cpp
  # core
  g2o/core/hyper_graph_action.cpp
  g2o/core/hyper_graph.cpp
  g2o/core/marginal_covariance_cholesky.cpp
  g2o/core/matrix_structure.cpp
  g2o/core/batch_stats.cpp
  g2o/core/parameter.cpp
  g2o/core/cache.cpp
  g2o/core/optimizable_graph.cpp
  g2o/core/solver.cpp
  g2o/core/optimization_algorithm_factory.cpp
  g2o/core/estimate_propagator.cpp
  g2o/core/factory.cpp
  g2o/core/sparse_optimizer.cpp
  g2o/core/hyper_dijkstra.cpp
  g2o/core/parameter_container.cpp
  g2o/core/optimization_algorithm.cpp
  g2o/core/optimization_algorithm_with_hessian.cpp
  g2o/core/optimization_algorithm_levenberg.cpp
  g2o/core/optimization_algorithm_gauss_newton.cpp
  g2o/core/jacobian_workspace.cpp
  g2o/core/robust_kernel.cpp
  g2o/core/robust_kernel_factory.cpp
  g2o/core/robust_kernel_impl.cpp
  # stuff
  g2o/stuff/timeutil.cpp
  g2o/stuff/os_specific.c
  g2o/stuff/string_tools.cpp
  g2o/stuff/property.cpp)

# Set up the top-level include directories
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC ${EIGEN3_INCLUDE_DIR}
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/types>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/stuff>
         $<INSTALL_INTERFACE:core>
         $<INSTALL_INTERFACE:types>
         $<INSTALL_INTERFACE:stuff>)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# Compiler specific options for gcc
target_compile_options(
  ${PROJECT_NAME}
  PRIVATE -O3
  PUBLIC -march=native)

if(OPENMP_FOUND AND G2O_USE_OPENMP)
  set(G2O_OPENMP 1)
  target_compile_options(
    ${PROJECT_NAME} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:-DEIGEN_DONT_PARALLELIZE;${OpenMP_CXX_FLAGS}>"
                            "$<$<COMPILE_LANGUAGE:C>:${OpenMP_C_FLAGS}>")
  message(STATUS "Compiling with OpenMP support")
endif(OPENMP_FOUND AND G2O_USE_OPENMP)

target_compile_options(${PROJECT_NAME} PUBLIC -march=native)

install(
  TARGETS g2o
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})
install(DIRECTORY ./ DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
