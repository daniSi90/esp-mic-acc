cmake_minimum_required(VERSION 3.10)
project(udp_catch VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

include(CTest)
enable_testing()

add_executable(udp_catch udp_catch.c)



 # Add include directories
target_include_directories(udp_catch PRIVATE
                                        ${CMAKE_SOURCE_DIR}/include
                                        )

link_directories(
    ${SDL2_LIB_DIR}
)

if (WIN32)
target_link_libraries(
    ${PROJECT_NAME}
        mingw32
)
else()
target_link_libraries(
    ${PROJECT_NAME}
       # mingw32
        m
)
endif (WIN32)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
