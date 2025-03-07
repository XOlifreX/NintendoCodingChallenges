cmake_minimum_required(VERSION 4.0)

project(NintendoCodingChallenges)
set(HIREME_PROJECT "HireMe")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g")

include_directories(include/hireme)
file(GLOB HIREME_SOURCES src/hireme/*.cpp include/hireme/*.h)

add_executable(${HIREME_PROJECT} ${HIREME_SOURCES})
target_link_libraries(${HIREME_PROJECT})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${HIREME_PROJECT})

set_target_properties(${HIREME_PROJECT} PROPERTIES COMPILE_FLAGS "/GL")
set_target_properties(${HIREME_PROJECT} PROPERTIES LINK_FLAGS "/LTCG:incremental")

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;DebugPrint;ReleasePrint" CACHE STRING "" FORCE) 
target_compile_definitions(${HIREME_PROJECT} PUBLIC
    $<$<CONFIG:DebugPrint>:PRINT_INFO>
    $<$<CONFIG:ReleasePrint>:PRINT_INFO>
)
