project(helloopenGL)

############################################
# Configure CMake and GCC flags
cmake_minimum_required(VERSION 2.8.9) # Minimal version compatible QT5
CMAKE_POLICY(SET CMP0043 NEW) # This will silence the Cmake Warning "Policy CMP0043 is not set"

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

find_package(Qt5 COMPONENTS OpenGL REQUIRED)

find_package(OpenGL REQUIRED) # define OPENGL_LIBRARIES

#OpenGL deprecated on MacosX : remove compilation warning
add_definitions(-DGL_SILENCE_DEPRECATION -DGLM_FORCE_SILENT_WARNINGS)

################################################################################
# Define project private sources and headers
#
# the variable "folder_source" contains all .cpp files of this project
FILE(GLOB_RECURSE
    folder_source
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/Geometry/*.cpp
)

FILE(GLOB_RECURSE
    folder_header
    ${CMAKE_SOURCE_DIR}/src/*.h
    ${CMAKE_SOURCE_DIR}/src/Geometry/*.h
)

FILE(GLOB_RECURSE
    folder_ui
    ${CMAKE_SOURCE_DIR}/src/*.ui
)

include_directories(
   ${CMAKE_SOURCE_DIR}/src
   ${CMAKE_SOURCE_DIR}/glm
)

SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})
SOURCE_GROUP("Shader Files" FILES ${folder_shader})

################################################################################
# Configure QT

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set( CMAKE_INCLUDE_CURRENT_DIR ON )


################################################################################
# Build target application

add_executable(helloopenGL
               ${folder_source}
               ${folder_header}
               ${folder_ui}
               )

set(EXT_LIBS Qt5::OpenGL ${OPENGL_LIBRARIES})

target_link_libraries(helloopenGL ${EXT_LIBS} )
