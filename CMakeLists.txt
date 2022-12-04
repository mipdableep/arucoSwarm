cmake_minimum_required(VERSION 3.1)
project(ArucoSwarm)

set(CMAKE_CXX_STANDARD 17)
find_package(Threads REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

add_library(tensorflowlite SHARED IMPORTED)
set_property(TARGET tensorflowlite PROPERTY IMPORTED_LOCATION
                                            /usr/local/lib/libtensorflowlite.so)
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED)
# find_package(CTello REQUIRED)

include_directories(include)

add_library(aruco src/aruco.cpp)
# add_library(detector src/detector.cc)
add_library(drone_client src/drone_client.cc)

target_link_libraries(aruco -lpthread ${OpenCV_LIBS} ${Boost_LIBRARIES})
# target_link_libraries(detector ${OpenCV_LIBS} ${Boost_LIBRARIES}
# tensorflowlite)

add_executable(followAruco followAruco.cpp)

target_link_libraries(
  followAruco aruco ctello
  # detector
  drone_client -lpthread nlohmann_json::nlohmann_json)

# set(CMAKE_BUILD_TYPE Debug)
