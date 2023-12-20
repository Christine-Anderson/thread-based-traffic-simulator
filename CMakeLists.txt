cmake_minimum_required(VERSION 3.8)

set(This pixel-game)

project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(SOURCE_CXX_SRC_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/source)

add_definitions(-Wno-inconsistent-missing-override)

# Source Files are Curated Here
file(
    GLOB_RECURSE _demoSourceFiles
    "${SOURCE_CXX_SRC_DIR}/*.cpp"
)

foreach(_demoSource IN LISTS _demoSourceFiles) #######################

get_filename_component(OutputExecutable ${_demoSource} NAME_WE)
add_executable(${OutputExecutable} ${_demoSource})

######################################################################
# MacOS
######################################################################
if(APPLE)
    
    # OpenGL
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)
    include_directories(${OpenGL_INCLUDE_DIRS})
    target_link_libraries(${OutputExecutable} ${OpenGL_LIBRARIES} OpenGL::GL)

    # Carbon
    FIND_LIBRARY(CARBON_LIBRARY Carbon)
    target_link_libraries(${OutputExecutable} ${CARBON_LIBRARY})

    # GLUT
    find_package(GLUT REQUIRED)
    target_link_libraries(${OutputExecutable} ${GLUT_LIBRARIES})

    # Threads
    find_package(Threads REQUIRED)
    target_link_libraries(${OutputExecutable} Threads::Threads)
    include_directories(${Threads_INCLUDE_DIRS})
    
    # SDL2_mixer
    set(USE_SDL2_MIXER ON)
    
    # PNG
    find_package(PNG REQUIRED)
    target_link_libraries(${OutputExecutable} PNG::PNG)
    include_directories(${PNG_INCLUDE_DIRS})
    
endif()

######################################################################
# Set include directory
######################################################################
if(IS_DIRECTORY ${SOURCE_CXX_INCLUDE_DIR})
    include_directories(${SOURCE_CXX_INCLUDE_DIR})
endif()

endforeach() #########################################################