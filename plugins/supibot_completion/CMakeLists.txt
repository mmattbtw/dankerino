project(supibot_completion)
cmake_minimum_required(VERSION 3.2.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(MAJOR_QT_VERSION "5")

#find_package(Qt5Widgets REQUIRED)
find_package(Qt${MAJOR_QT_VERSION} REQUIRED
    COMPONENTS
    Core
    Widgets
    Network
    Concurrent
    )

include_directories(../../src)
set(COMPILE_OPTIONS -std=c++17)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../built/)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../built/)

add_library(supibot_completion SHARED supibot_completion.h)
target_link_libraries(supibot_completion
        PRIVATE
        Qt${MAJOR_QT_VERSION}::Core
        Qt${MAJOR_QT_VERSION}::Widgets
        Qt${MAJOR_QT_VERSION}::Network
        Qt${MAJOR_QT_VERSION}::Concurrent
        )
target_compile_options(supibot_completion PRIVATE ${COMPILE_OPTIONS})

#add_executable(test-plugin-exec main.cpp)
#target_link_libraries(test-plugin-exec PRIVATE Qt5::Widgets)
#target_compile_options(test-plugin-exec PRIVATE ${COMPILE_OPTIONS})

