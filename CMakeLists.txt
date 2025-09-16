cmake_minimum_required(VERSION 3.20)
project(RCC_CVRP LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Gurobi REQUIRED)

add_library(cvrpsep)
add_subdirectory(RCC_CVRP/cvrpsep)

target_include_directories(cvrpsep
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/RCC_CVRP
        ${CMAKE_CURRENT_SOURCE_DIR}/RCC_CVRP/cvrpsep
)

add_executable(RCC_CVRP_exe
    RCC_CVRP/main.cpp
)

target_include_directories(RCC_CVRP_exe
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/RCC_CVRP
)

target_link_libraries(RCC_CVRP_exe
    PRIVATE
        cvrpsep
        Gurobi::Gurobi
)

target_compile_definitions(RCC_CVRP_exe
    PRIVATE
        CVRP_DEFAULT_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Instances"
)

if (APPLE)
    target_link_libraries(RCC_CVRP_exe PRIVATE m)
elseif (UNIX)
    target_link_libraries(RCC_CVRP_exe PRIVATE m)
endif()

set_target_properties(RCC_CVRP_exe PROPERTIES OUTPUT_NAME cvrp_solver)
