cmake_minimum_required(VERSION 3.8)

project(Server)

set(ASIO_DIRECTORY "../Dependencies/ASIO/asio/include")
set(PROJECT_SOURCES_DIR "./src")
set(PROJECT_INCLUDES_DIR "./include")
set(BASE_PROJECT_INCLUDES_DIR "../Common/include")

include_directories(
        ${PROJECT_INCLUDES_DIR}
        ${BASE_PROJECT_INCLUDES_DIR}
        ${ASIO_DIRECTORY}
)

file(GLOB PROJECT_SOURCES ${PROJECT_SOURCES_DIR}/*.cpp)

add_executable(Server ${PROJECT_SOURCES})
set_property(TARGET Server PROPERTY RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")