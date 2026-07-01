cmake_minimum_required(VERSION 3.16)

project(NyxWatch-server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)
find_package(Qt6 REQUIRED COMPONENTS Network)

add_executable(NyxWatch-server main.cpp
    camerastream.h camerastream.cpp
    networkserver.h networkserver.cpp
    audiostream.h audiostream.cpp)

target_link_libraries(NyxWatch-server PRIVATE Qt6::Core Qt6::Multimedia Qt6::Network)

include(GNUInstallDirs)
install(TARGETS NyxWatch-server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
