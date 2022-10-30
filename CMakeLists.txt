cmake_minimum_required(VERSION 3.1)
project(2022summerCourse)

set(CMAKE_CXX_STANDARD 17)
find_package(Threads REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

find_package(OpenCV 3.0 QUIET)

include_directories(include)

add_library(aruco src/aruco.cpp)
add_library(drone src/drone.cpp)

target_link_libraries(drone -lctello -lpthread ${OpenCV_LIBS})

#add_executable(cameraCalib cameraCalib.cpp)
#target_link_libraries(cameraCalib aruco drone -lctello -lpthread nlohmann_json::nlohmann_json )


add_executable(runAruco runAruco.cpp)
target_link_libraries(runAruco aruco drone -lctello -lpthread nlohmann_json::nlohmann_json )
