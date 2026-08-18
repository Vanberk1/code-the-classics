cmake_minimum_required(VERSION 3.20)
project(pong)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES 
    src/*.cpp 
    src/*.h
)

add_executable(pong ${SOURCES})

target_link_directories(pong PRIVATE
    ${CMAKE_SOURCE_DIR}/../lib/pudu/lib
)

# Link against pudu
target_link_libraries(pong PRIVATE 
    pudu
    opengl32
    gdi32
)

# Include directories
target_include_directories(pong PRIVATE 
    ${CMAKE_SOURCE_DIR}/../lib/pudu/include
    ${CMAKE_SOURCE_DIR}/../lib/pudu/include/pudu
    ${CMAKE_SOURCE_DIR}/../lib/glm/include
)