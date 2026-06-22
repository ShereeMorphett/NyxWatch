cmake_minimum_required(VERSION 3.16)

project(NyxWatch_client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.10)


find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)

qt_add_executable(appNyxWatch_client
    main.cpp
)

qt_add_qml_module(appNyxWatch_client
    URI NyxWatch_client
    QML_FILES
        Main.qml
        SOURCES clientstream.h clientstream.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appNyxWatch_client PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appNyxWatch_client
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appNyxWatch_client
    PRIVATE
    Qt6::Quick
    Qt6::Multimedia
)

install(TARGETS appNyxWatch_client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
