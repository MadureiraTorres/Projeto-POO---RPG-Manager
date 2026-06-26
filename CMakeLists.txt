cmake_minimum_required(VERSION 3.14)
project(RPGManager CXX)
set(CMAKE_CXX_STANDARD 17)

add_executable(rpg_manager main.cpp)
target_include_directories(rpg_manager PRIVATE src)
target_link_libraries(rpg_manager sqlite3)

enable_testing()
add_subdirectory(tests)
