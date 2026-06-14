cmake_minimum_required(VERSION 3.16)

project(fts_simulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_SOURCES
    main.cpp
    map.cpp
    Robot.cpp
    RobotController.cpp
    ObstacleManager.cpp
    LidarSensor.cpp
)

set(PROJECT_HEADERS
    map.h
    Robot.h
    RobotController.h
    ObstacleManager.h
    LidarSensor.h
)

add_executable(robot_sim ${PROJECT_SOURCES} ${PROJECT_HEADERS})

find_package(raylib QUIET)

if(raylib_FOUND)
    target_link_libraries(robot_sim PRIVATE raylib)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(RAYLIB REQUIRED raylib)

    target_include_directories(robot_sim PRIVATE ${RAYLIB_INCLUDE_DIRS})
    target_link_directories(robot_sim PRIVATE ${RAYLIB_LIBRARY_DIRS})
    target_compile_options(robot_sim PRIVATE ${RAYLIB_CFLAGS_OTHER})
    target_link_libraries(robot_sim PRIVATE ${RAYLIB_LIBRARIES})
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(robot_sim PRIVATE m dl pthread GL rt X11)
endif()

set(FORMAT_FILES ${PROJECT_SOURCES} ${PROJECT_HEADERS})

find_program(CLANG_FORMAT_EXE clang-format)
if(CLANG_FORMAT_EXE)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -i ${FORMAT_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Formatting source files"
    )

    add_custom_target(format-check
        COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${FORMAT_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Checking source formatting"
    )
endif()

find_program(CLANG_TIDY_EXE clang-tidy)
if(CLANG_TIDY_EXE)
    add_custom_target(lint
        COMMAND bash tools/run-clang-tidy.sh -p ${CMAKE_BINARY_DIR} --quiet --warnings-as-errors=* ${PROJECT_SOURCES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running clang-tidy"
    )

    add_custom_target(lint-fix
        COMMAND bash tools/run-clang-tidy.sh -p ${CMAKE_BINARY_DIR} --fix --quiet --warnings-as-errors=* ${PROJECT_SOURCES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running clang-tidy with automatic fixes"
    )
endif()
