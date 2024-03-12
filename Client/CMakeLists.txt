cmake_minimum_required(VERSION 3.8)

project(Client)

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
list(REMOVE_ITEM PROJECT_SOURCES "${PROJECT_SOURCES_DIR}/main.cpp")

add_library(Client STATIC ${PROJECT_SOURCES})