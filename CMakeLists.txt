cmake_minimum_required(VERSION 3.5)

project(guessDogCat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# --- OpenCV --------------------
# Trỏ tới sysroot đã rsync header + CMake config
set(OpenCV_DIR /home/quang/rpi-sysroot/usr/local/lib/aarch64-linux-gnu/cmake/opencv4)
find_package(OpenCV REQUIRED)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "Không tìm thấy OpenCV")
endif()
message(STATUS "Found OpenCV version: ${OpenCV_VERSION}")
include_directories(${OpenCV_INCLUDE_DIRS})
# -------------------------------
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        resource.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(guessDogCat
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET guessDogCat APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(guessDogCat SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(guessDogCat
            ${PROJECT_SOURCES}
        )
    endif()
endif()
# --------------------------------------------------------------------------------
#              			 --- Wiring Pi library set up ---
find_library(WIRINGPI_LIB wiringPi PATHS /usr/lib)

if(WIRINGPI_LIB)
    target_link_libraries(guessDogCat PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        ${OpenCV_LIBS}
        ${WIRINGPI_LIB}
        pthread
        dl
        m
        rt
    )
else()
    message(FATAL_ERROR "Không tìm thấy thư viện wiringPi")
endif()
# --------------------------------------------------------------------------------
set_target_properties(guessDogCat PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS guessDogCat
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(guessDogCat)
endif()
