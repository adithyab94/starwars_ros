cmake_minimum_required(VERSION 2.8.3)
project(star_wars)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  geometry_msgs
  roscpp
  tf
)

catkin_package(
)


include_directories(
  ${catkin_INCLUDE_DIRS}
)

## Declare a C++ executable
add_executable(controller_1_node src/controller_1_node.cpp)
add_executable(controller_2_node src/controller_2_node.cpp)

## Specify libraries to link a library or executable target against
target_link_libraries(controller_1_node   ${catkin_LIBRARIES} )
target_link_libraries(controller_2_node   ${catkin_LIBRARIES} )
