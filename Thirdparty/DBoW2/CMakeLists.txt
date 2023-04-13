cmake_minimum_required(VERSION 3.16.3)
project(DBoW2)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(OpenCV 4.2 QUIET)

add_library(
  ${PROJECT_NAME} SHARED
  # DBoW2
  src/DBoW2/BowVector.cpp
  src/DBoW2/FORB.cpp
  src/DBoW2/FeatureVector.cpp
  src/DBoW2/ScoringObject.cpp
  # DUtils
  src/DUtils/Random.cpp
  src/DUtils/Timestamp.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                                  $<INSTALL_INTERFACE:include> ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 14)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -O3 -march=native)

install(
  TARGETS DBoW2
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})
install(DIRECTORY include/ DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
