cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(
    deepshark
    VERSION 1.0
    LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    #set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(Torch REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

add_library(_deepshark SHARED 
    src/cards.cpp 
    inc/cards.hpp 
    src/deepshark.cpp
    inc/deepshark.hpp
    src/playback.cpp
    inc/playback.hpp
    src/player.cpp
    inc/player.hpp
    src/pot_dealer.cpp
    inc/pot_dealer.hpp
    src/serialize.cpp
    inc/serialize.hpp
    src/storage.cpp
    inc/storage.hpp
    src/texasholdem.cpp
    inc/texasholdem.hpp
    inc/constants.hpp
    inc/tabulate/tabulate.hpp)

target_include_directories(_deepshark PUBLIC inc)
target_compile_features(_deepshark PUBLIC cxx_std_23)
target_compile_options(_deepshark PRIVATE -Werror -Wall -Wextra -Wconversion)

target_link_libraries(_deepshark "${TORCH_LIBRARIES}")
target_link_libraries(_deepshark ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_playback 
    tests/functional/test_playback.cpp
    src/cards.cpp 
    inc/cards.hpp
    src/playback.cpp
    inc/playback.hpp
    src/player.cpp
    inc/player.hpp
    src/pot_dealer.cpp
    inc/pot_dealer.hpp
    src/serialize.cpp
    inc/serialize.hpp
    src/storage.cpp
    inc/storage.hpp
    src/texasholdem.cpp
    inc/texasholdem.hpp
    inc/constants.hpp
    inc/tabulate/tabulate.hpp)

target_include_directories(test_playback PUBLIC inc)

target_compile_features(test_playback PUBLIC cxx_std_23)
target_compile_options(test_playback PRIVATE -Werror -Wall -Wextra -Wconversion)

target_link_libraries(test_playback "${TORCH_LIBRARIES}")