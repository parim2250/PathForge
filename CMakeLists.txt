cmake_minimum_required(VERSION 3.16)
project(PathForge VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR "C:/SFML/lib/cmake/SFML")

find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(PathForge ${SOURCES})

target_link_libraries(PathForge 
    sfml-graphics 
    sfml-window 
    sfml-system
)

target_include_directories(PathForge PRIVATE src)

if(WIN32)
    add_custom_command(TARGET PathForge POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "C:/SFML/bin"
        $<TARGET_FILE_DIR:PathForge>
    )
endif()