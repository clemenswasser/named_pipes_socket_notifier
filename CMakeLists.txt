cmake_minimum_required(VERSION 3.26)

project(named_pipes_socket_notifier LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core)
qt_standard_project_setup()


qt_add_executable(named_pipes_socket_notifier src/main.cpp)
target_compile_features(named_pipes_socket_notifier PRIVATE cxx_std_20)
target_link_libraries(named_pipes_socket_notifier PRIVATE Qt6::Core)