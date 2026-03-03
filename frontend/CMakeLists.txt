cmake_minimum_required(VERSION 3.16)

project(bank-automat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
main.cpp
mainwindow.cpp
mainwindow.h
mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
qt_add_executable(bank-automat
MANUAL_FINALIZATION
${PROJECT_SOURCES}

environment.h environment.cpp

    kortinvalintawindow.h kortinvalintawindow.cpp kortinvalintawindow.ui

    debitwindow.h debitwindow.cpp debitwindow.ui

    creditwindow.h creditwindow.cpp creditwindow.ui

    nostodebit.h nostodebit.cpp nostodebit.ui

    tilitapahtumatwindow.h tilitapahtumatwindow.cpp tilitapahtumatwindow.ui

    taustakuvalogin.qrc
    idlemanager.h idlemanager.cpp
    config.h
)

else()
if(ANDROID)
add_library(bank-automat SHARED
${PROJECT_SOURCES}
)
else()
add_executable(bank-automat
${PROJECT_SOURCES}
)
endif()
endif()

target_link_libraries(bank-automat
PRIVATE
Qt${QT_VERSION_MAJOR}::Widgets
Qt${QT_VERSION_MAJOR}::Network
)

if(${QT_VERSION} VERSION_LESS 6.1.0)
set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.bank-automat)
endif()

set_target_properties(bank-automat PROPERTIES
${BUNDLE_ID_OPTION}
MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
MACOSX_BUNDLE TRUE
WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)

install(TARGETS bank-automat
BUNDLE DESTINATION .
LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
qt_finalize_executable(bank-automat)
endif()
