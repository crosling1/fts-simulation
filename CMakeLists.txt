cmake_minimum_required(VERSION 3.16)

project(fts_simulation LANGUAGES CXX)

if(POLICY CMP0110)
    cmake_policy(SET CMP0110 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_SOURCES
    src/main.cpp
    src/simulation/MapData.cpp
    src/simulation/map.cpp
    src/simulation/navigation.cpp
    src/simulation/InputState.cpp
    src/chargingmanager.cpp
    src/emergencystopcontroller.cpp
    src/routefollower.cpp
    src/simulation/RobotController.cpp
    src/simulation/RobotRoutePlanner.cpp
    src/simulation/RobotTaskFlow.cpp
    src/simulation/BlockingRobotManager.cpp
    src/ui/StatusOverlay.cpp
    src/robots/Battery.cpp
    src/control/PIController.cpp
    src/robots/Robot.cpp
    src/robots/WorkerRobot.cpp
    src/sensors/ProximitySensor.cpp
)

set(COMMON_SOURCES
    src/simulation/MapData.cpp
    src/simulation/map.cpp
    src/simulation/navigation.cpp
    src/simulation/BlockingRobotManager.cpp
    src/robots/Battery.cpp
    src/control/PIController.cpp
    src/robots/Robot.cpp
    src/robots/WorkerRobot.cpp
    src/sensors/ProximitySensor.cpp
)

set(TEST_SOURCES
    tests/unit_tests.cpp
    tests/robots/robot_tests.cpp
    tests/robots/pi_controller_tests.cpp
    tests/sensors/proximity_sensor_tests.cpp
    tests/simulation/blocking_robot_manager_tests.cpp
    tests/simulation/map_tests.cpp
    tests/simulation/navigation_tests.cpp
)

set(PROJECT_HEADERS
    include/robots/Robot.h
    include/robots/Battery.h
    include/control/PIController.h
    include/robots/WorkerRobot.h
    include/sensors/Sensor.h
    include/sensors/ProximitySensor.h
    include/simulation/MapData.h
    include/simulation/map.h
    include/simulation/navigation.h
    include/simulation/InputState.h
    include/chargingmanager.h
    include/emergencystopcontroller.h
    include/routefollower.h
    include/simulation/RobotController.h
    include/simulation/RobotRoutePlanner.h
    include/simulation/RobotStatusSnapshot.h
    include/simulation/RobotTaskFlow.h
    include/simulation/BlockingRobotManager.h
    include/ui/StatusOverlay.h
)

add_executable(robot_sim ${PROJECT_SOURCES} ${PROJECT_HEADERS})
target_include_directories(robot_sim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(unit_tests ${TEST_SOURCES} ${COMMON_SOURCES} ${PROJECT_HEADERS})
target_include_directories(unit_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

enable_testing()

set(UNIT_TEST_CASES
    "Robot moves to target"
    "Robot PI controller limits speed near target"
    "Robot keeps carrying state when arriving"
    "Robot rotates toward target"
    "Robot owns battery"
    "Robot drains battery by distance moved"
    "Robot stops when battery is empty"
    "PI controller uses max output without gains"
    "PI controller clamps output"
    "PI controller reset clears integral"
    "Blocking robot detection ignores inactive robots"
    "Blocking robot outside radius is not detected"
    "Blocking robot inside radius is detected"
    "Blocking robot moves along path"
    "Proximity sensor uses blocking robot manager"
    "Map road queries"
    "Navigation finds warehouse routes"
)

foreach(test_case IN LISTS UNIT_TEST_CASES)
    add_test(NAME "${test_case}" COMMAND unit_tests --test "${test_case}")
endforeach()

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
