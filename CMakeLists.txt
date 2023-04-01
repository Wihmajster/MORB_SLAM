cmake_minimum_required(VERSION 3.16.3)
project(
  MORB_SLAM
  VERSION 1.0
  LANGUAGES CXX)

# Release by default Turn on Debug with "-DCMAKE_BUILD_TYPE=Debug"
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(catkin REQUIRED COMPONENTS roscpp)
find_package(OpenCV 4.2 REQUIRED)
find_package(Eigen3 3.1.0 REQUIRED)

catkin_package(
  INCLUDE_DIRS
  include
  include/MORB_SLAM
  include/MORB_SLAM/CameraModels
  Thirdparty/DBoW2
  Thirdparty/g2o
  Thirdparty/Sophus
  LIBRARIES
  ${PROJECT_NAME}
  DBoW2
  g2o
  CATKIN_DEPENDS
  roscpp
  DEPENDS
  EIGEN3
  OpenCV)

add_subdirectory(Thirdparty/g2o)
add_subdirectory(Thirdparty/DBoW2)
add_subdirectory(Thirdparty/Sophus)

add_library(
  ${PROJECT_NAME} 
  src/System.cc
  src/Tracking.cc
  src/LocalMapping.cc
  src/LoopClosing.cc
  src/ORBextractor.cc
  src/ORBmatcher.cc
  src/Converter.cc
  src/MapPoint.cc
  src/KeyFrame.cc
  src/Atlas.cc
  src/Map.cc
  src/Optimizer.cc
  src/Frame.cc
  src/KeyFrameDatabase.cc
  src/Sim3Solver.cc
  src/ImuTypes.cc
  src/G2oTypes.cc
  src/CameraModels/Pinhole.cpp
  src/CameraModels/KannalaBrandt8.cpp
  src/OptimizableTypes.cpp
  src/MLPnPsolver.cpp
  src/GeometricTools.cc
  src/TwoViewReconstruction.cc
  src/Config.cc
  src/Settings.cc)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/MORB_SLAM>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/MORB_SLAM/CameraModels>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty/DBoW2/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty/Sophus>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty/g2o>
         $<BUILD_INTERFACE:${EIGEN3_INCLUDE_DIRS}>
        ${catkin_INCLUDE_DIRS})

target_link_libraries(
  ${PROJECT_NAME}
  ${OpenCV_LIBS}
  ${EIGEN3_LIBS}
  DBoW2
  g2o
  -lboost_serialization
  -lcrypto)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

if(CMAKE_BUILD_TYPE EQUAL "Release" OR CMAKE_BUILD_TYPE EQUAL "RelWithDebInfo")
  target_compile_options(
    orb_slam3
    PRIVATE -O3
    PUBLIC -march=native)
endif()

add_custom_target(vocabulary ALL)
add_custom_command(
  TARGET vocabulary
  PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/ORBvoc.txt.tar.gz
  COMMAND ${CMAKE_COMMAND} -E tar xf
          ${PROJECT_SOURCE_DIR}/Vocabulary/ORBvoc.txt.tar.gz
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DEPENDS ${PROJECT_SOURCE_DIR}/Vocabulary/ORBvoc.txt.tar.gz
  COMMENT "Unpacking ORBvoc.txt.tar.gz"
  VERBATIM)
add_dependencies(${PROJECT_NAME} vocabulary)

# include(GNUInstallDirs) set(INSTALL_CONFIGDIR
# ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}) install( TARGETS
# ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-targets LIBRARY DESTINATION
# ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}) install(
# DIRECTORY ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include
# ${PROJECT_SOURCE_DIR}/include/CameraModels
# ${PROJECT_SOURCE_DIR}/Thirdparty/Sophus ${EIGEN3_INCLUDE_DIR}
# ${PROJECT_SOURCE_DIR}/Thirdparty/DBoW2/DBoW2 DESTINATION
# ${CMAKE_INSTALL_INCLUDEDIR})

# install( EXPORT ${PROJECT_NAME}-targets FILE ${PROJECT_NAME}Targets.cmake
# NAMESPACE ${PROJECT_NAME}:: DESTINATION ${INSTALL_CONFIGDIR})

# include(CMakePackageConfigHelpers) write_basic_package_version_file(
# ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake VERSION
# ${PROJECT_VERSION} COMPATIBILITY AnyNewerVersion)

# configure_package_config_file(
# ${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
# ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake INSTALL_DESTINATION
# ${INSTALL_CONFIGDIR})

# install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
# ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake DESTINATION
# ${INSTALL_CONFIGDIR})

# export( EXPORT ${PROJECT_NAME}-targets FILE
# ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake NAMESPACE
# ${PROJECT_NAME}::)

# export(PACKAGE ${PROJECT_NAME})
