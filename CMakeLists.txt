cmake_minimum_required(VERSION 3.1)
project(ArucoSwarm)

set(CMAKE_CXX_STANDARD 17)
find_package(Threads REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

add_library(tensorflowlite SHARED IMPORTED)
set_property(TARGET tensorflowlite PROPERTY IMPORTED_LOCATION
                                         /usr/lib/aarch64-linux-gnu/libtensorflowlite.so)
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED)
# find_package(CTello REQUIRED)

include_directories(include)

add_library(aruco src/aruco.cpp)
add_library(drone src/drone.cpp)
add_library(detector src/detector.cc)

target_link_libraries(drone -lctello -lpthread ${OpenCV_LIBS})
target_link_libraries(aruco -lpthread ${OpenCV_LIBS} ${Boost_LIBRARIES})
target_link_libraries(detector ${OpenCV_LIBS} ${Boost_LIBRARIES} tensorflowlite)

add_executable(runAruco runAruco.cpp)

target_link_libraries(
  runAruco
  aruco
  drone
  ctello
  detector
  -lpthread
  nlohmann_json::nlohmann_json)
