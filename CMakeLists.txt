cmake_minimum_required(VERSION 3.16)

project(fts_simulation LANGUAGES CXX)

if(POLICY CMP0110)
    cmake_policy(SET CMP0110 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(FTS_PROJECT_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY}")
set(CMAKE_CXX_CLANG_TIDY "")

include(FetchContent)

FetchContent_Declare(
    Catch2
    URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.7.1.tar.gz
)
FetchContent_MakeAvailable(Catch2)

function(fts_mark_target_includes_as_system target_name)
    get_target_property(target_includes ${target_name} INTERFACE_INCLUDE_DIRECTORIES)
    if(target_includes)
        set_target_properties(${target_name} PROPERTIES
            INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${target_includes}"
        )
    endif()
endfunction()

foreach(catch2_target IN ITEMS Catch2 Catch2WithMain)
    if(TARGET ${catch2_target})
        set_target_properties(${catch2_target} PROPERTIES CXX_CLANG_TIDY "")
        fts_mark_target_includes_as_system(${catch2_target})
    endif()
endforeach()

set(FTS_CORE_SOURCES
    src/simulation/MapData.cpp
    src/simulation/NavigationGraph.cpp
    src/simulation/navigation.cpp
    src/simulation/ChargingManager.cpp
    src/simulation/EmergencyStopController.cpp
    src/simulation/RobotController.cpp
    src/simulation/RobotRoutePlanner.cpp
    src/simulation/RobotTaskFlow.cpp
    src/robots/Battery.cpp
    src/control/PIController.cpp
    src/robots/Robot.cpp
    src/robots/WorkerRobot.cpp
    src/sensors/ProximitySensor.cpp
)

set(FTS_CORE_BOUNDARY_SOURCES
    src/simulation/map.cpp
    src/simulation/RouteFollower.cpp
    src/simulation/BlockingRobotManager.cpp
)

list(APPEND FTS_CORE_SOURCES
    ${FTS_CORE_BOUNDARY_SOURCES}
)

set(FTS_TEST_SOURCES
    tests/robots/robot_tests.cpp
    tests/robots/pi_controller_tests.cpp
    tests/sensors/proximity_sensor_tests.cpp
    tests/simulation/blocking_robot_manager_tests.cpp
    tests/simulation/charging_manager_tests.cpp
    tests/simulation/emergency_stop_controller_tests.cpp
    tests/simulation/map_tests.cpp
    tests/simulation/navigation_tests.cpp
    tests/simulation/robot_controller_tests.cpp
    tests/simulation/robot_route_planner_tests.cpp
    tests/simulation/robot_task_flow_tests.cpp
)

set(FTS_CORE_HEADERS
    include/robots/Robot.h
    include/robots/IRobotRenderer.h
    include/robots/RobotRenderData.h
    include/robots/RobotState.h
    include/robots/Battery.h
    include/control/PIController.h
    include/robots/WorkerRobot.h
    include/sensors/Sensor.h
    include/sensors/ProximitySensor.h
    include/simulation/ILogisticsMap.h
    include/simulation/MapData.h
    include/simulation/Geometry.h
    include/simulation/SimConfig.h
    include/simulation/NavigationGraph.h
    include/simulation/navigation.h
    include/simulation/ChargingManager.h
    include/simulation/EmergencyStopController.h
    include/simulation/RobotController.h
    include/simulation/RobotRoutePlanner.h
    include/simulation/RobotStatusSnapshot.h
    include/simulation/RobotTaskFlow.h
    include/simulation/MathUtils.h
)

set(FTS_CORE_BOUNDARY_HEADERS
    include/simulation/map.h
    include/simulation/InputState.h
    include/simulation/RouteFollower.h
    include/simulation/BlockingRobotManager.h
)

list(APPEND FTS_CORE_HEADERS
    ${FTS_CORE_BOUNDARY_HEADERS}
)

set(FTS_RENDERING_SOURCES
    src/rendering/BlockingRobotRenderer.cpp
    src/rendering/LogisticsMapRenderer.cpp
    src/rendering/RaylibInputState.cpp
    src/rendering/RouteFollowerRenderer.cpp
    src/rendering/RobotControllerRenderer.cpp
    src/rendering/RobotRenderer.cpp
    src/ui/MapOverlay.cpp
    src/ui/StatusOverlay.cpp
)

set(FTS_RENDERING_HEADERS
    include/rendering/RaylibGeometry.h
    include/rendering/RobotRenderer.h
    include/rendering/SimulationRenderer.h
    include/ui/MapOverlay.h
    include/ui/StatusOverlay.h
)

add_library(fts_core ${FTS_CORE_SOURCES} ${FTS_CORE_HEADERS})
target_include_directories(fts_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(fts_rendering ${FTS_RENDERING_SOURCES} ${FTS_RENDERING_HEADERS})
target_link_libraries(fts_rendering PUBLIC fts_core)

add_executable(robot_sim src/main.cpp)
target_link_libraries(robot_sim PRIVATE fts_core fts_rendering)

add_executable(unit_tests ${FTS_TEST_SOURCES})
target_link_libraries(unit_tests PRIVATE fts_core Catch2::Catch2WithMain)
target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tests)

if(FTS_PROJECT_CLANG_TIDY)
    set_target_properties(fts_core fts_rendering robot_sim unit_tests PROPERTIES
        CXX_CLANG_TIDY "${FTS_PROJECT_CLANG_TIDY}"
    )
endif()

enable_testing()

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(unit_tests)

find_package(raylib QUIET)

if(raylib_FOUND)
    target_link_libraries(fts_rendering PRIVATE raylib)
    target_link_libraries(robot_sim PRIVATE raylib)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(RAYLIB REQUIRED raylib)

    target_include_directories(fts_rendering PRIVATE ${RAYLIB_INCLUDE_DIRS})
    target_link_directories(fts_rendering PRIVATE ${RAYLIB_LIBRARY_DIRS})
    target_compile_options(fts_rendering PRIVATE ${RAYLIB_CFLAGS_OTHER})
    target_link_libraries(fts_rendering PRIVATE ${RAYLIB_LIBRARIES})

    target_include_directories(robot_sim PRIVATE ${RAYLIB_INCLUDE_DIRS})
    target_link_directories(robot_sim PRIVATE ${RAYLIB_LIBRARY_DIRS})
    target_compile_options(robot_sim PRIVATE ${RAYLIB_CFLAGS_OTHER})
    target_link_libraries(robot_sim PRIVATE ${RAYLIB_LIBRARIES})
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(fts_rendering PRIVATE m dl pthread GL rt X11)
    target_link_libraries(robot_sim PRIVATE m dl pthread GL rt X11)
endif()
