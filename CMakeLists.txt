cmake_minimum_required(VERSION 3.16)

project(fts_simulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_SOURCES
    src/main.cpp
    src/simulation/MapData.cpp
    src/simulation/map.cpp
    src/simulation/navigation.cpp
    src/simulation/BlockingRobotController.cpp
    src/simulation/RobotController.cpp
    src/simulation/ObstacleManager.cpp
    src/ui/StatusOverlay.cpp
    src/robots/Battery.cpp
    src/robots/Robot.cpp
    src/robots/WorkerRobot.cpp
    src/robots/ObstacleRobot.cpp
    src/sensors/LidarSensor.cpp
)

set(COMMON_SOURCES
    src/simulation/MapData.cpp
    src/simulation/map.cpp
    src/simulation/navigation.cpp
    src/simulation/ObstacleManager.cpp
    src/robots/Battery.cpp
    src/robots/Robot.cpp
    src/robots/WorkerRobot.cpp
    src/robots/ObstacleRobot.cpp
    src/sensors/LidarSensor.cpp
)

set(PROJECT_HEADERS
    include/robots/Robot.h
    include/robots/Battery.h
    include/robots/WorkerRobot.h
    include/robots/ObstacleRobot.h
    include/sensors/Sensor.h
    include/sensors/LidarSensor.h
    include/simulation/MapData.h
    include/simulation/map.h
    include/simulation/navigation.h
    include/simulation/BlockingRobotController.h
    include/simulation/RobotController.h
    include/simulation/RobotStatusSnapshot.h
    include/simulation/ObstacleManager.h
    include/ui/StatusOverlay.h
)

add_executable(robot_sim ${PROJECT_SOURCES} ${PROJECT_HEADERS})
target_include_directories(robot_sim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(unit_tests tests/unit_tests.cpp ${COMMON_SOURCES} ${PROJECT_HEADERS})
target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

enable_testing()
add_test(NAME unit_tests COMMAND unit_tests)

find_package(raylib QUIET)

if(raylib_FOUND)
    target_link_libraries(robot_sim PRIVATE raylib)
    target_link_libraries(unit_tests PRIVATE raylib)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(RAYLIB REQUIRED raylib)

    target_include_directories(robot_sim PRIVATE ${RAYLIB_INCLUDE_DIRS})
    target_include_directories(unit_tests PRIVATE ${RAYLIB_INCLUDE_DIRS})
    target_link_directories(robot_sim PRIVATE ${RAYLIB_LIBRARY_DIRS})
    target_link_directories(unit_tests PRIVATE ${RAYLIB_LIBRARY_DIRS})
    target_compile_options(robot_sim PRIVATE ${RAYLIB_CFLAGS_OTHER})
    target_compile_options(unit_tests PRIVATE ${RAYLIB_CFLAGS_OTHER})
    target_link_libraries(robot_sim PRIVATE ${RAYLIB_LIBRARIES})
    target_link_libraries(unit_tests PRIVATE ${RAYLIB_LIBRARIES})
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(robot_sim PRIVATE m dl pthread GL rt X11)
    target_link_libraries(unit_tests PRIVATE m dl pthread GL rt X11)
endif()
