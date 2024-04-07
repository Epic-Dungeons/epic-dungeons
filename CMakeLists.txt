cmake_minimum_required(VERSION 3.18)
project(EpicDungeons)

set(CMAKE_CXX_STANDARD 20)

file (GLOB_RECURSE HEADERS src/*.h)
# both cpp and c files
file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)

add_executable(EpicDungeons main.cpp ${SOURCES} ${HEADERS})
target_include_directories(EpicDungeons PRIVATE src)

# add spdlog
find_package(spdlog 1.8.1 REQUIRED)
target_link_libraries(EpicDungeons PRIVATE spdlog::spdlog)

# add chrono
find_package(Threads REQUIRED)
target_link_libraries(EpicDungeons PRIVATE Threads::Threads)

# add sfml
# set(SFML_DIR "/opt/homebrew/Cellar/sfml/2.5.1")
find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)
target_link_libraries(EpicDungeons PRIVATE sfml-graphics sfml-audio)

# unpack gif into frames
if (NOT DEFINED CLEAN_RES)
    set(CLEAN_RES FALSE)
endif()

#clean res folder
if (CLEAN_RES)
    file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/res)
endif()

# include res folder
file(COPY ${CMAKE_SOURCE_DIR}/res DESTINATION ${CMAKE_BINARY_DIR})

# unpack gif into frames
if (NOT DEFINED UNPACK_GIFS)
    set(UNPACK_GIFS FALSE)
endif()

if (UNPACK_GIFS)
    file(GLOB_RECURSE GIFS ${CMAKE_SOURCE_DIR}/res/*.gif)
    foreach(GIF ${GIFS})
        message(STATUS "GIF: ${GIF}")
        file(RELATIVE_PATH RELATIVE_GIF ${CMAKE_SOURCE_DIR}/res ${GIF})
        string(REPLACE ".gif" "" FRAME_DIR ${RELATIVE_GIF})
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/res/${FRAME_DIR})
        execute_process(COMMAND convert -coalesce ${GIF} ${CMAKE_BINARY_DIR}/res/${FRAME_DIR}/frame.png)
    endforeach()
endif()

# add define for debug mode
if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(EpicDungeons PRIVATE DEBUG)
endif()

# add define for release mode
if (CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_definitions(EpicDungeons PRIVATE RELEASE)
endif()

# add define for profiling mode
if (CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    target_compile_definitions(EpicDungeons PRIVATE PROFILING)
endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    # add test executable
    file(GLOB_RECURSE PARSING_SOURCES src/engine/parsing/*.cpp)
    add_executable(ParserTest test/parse.cpp ${PARSING_SOURCES})
    target_include_directories(ParserTest PRIVATE src)
    target_link_libraries(ParserTest PRIVATE spdlog::spdlog)
    target_link_libraries(EpicDungeons PRIVATE Threads::Threads)
endif()