cmake_minimum_required(VERSION 3.10)

project(ReleaseInstaller LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

add_subdirectory(tests)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS Concurrent REQUIRED)
find_package(LibArchive REQUIRED)
find_package(CURL REQUIRED)

if(ANDROID)
  add_library(ReleaseInstaller SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    Assets/assetmanagerbase.cpp
    Assets/assetmanagerbase.h
          Assets/github/githubassetmanager.cpp
          Assets/github/githubassetmanager.h
    Assets/download.h
    Assets/download.cpp
    Assets/progressdialog.cpp
    Assets/progressdialog.h
    Assets/progressdialog.ui
    Archives/zippackage.cpp
    Archives/zippackage.h
  )
else()
  add_executable(ReleaseInstaller
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    Assets/assetmanagerbase.cpp
    Assets/assetmanagerbase.h
          Assets/github/githubassetmanager.cpp
          Assets/github/githubassetmanager.h
    Assets/download.h
    Assets/download.cpp
    Assets/progressdialog.cpp
    Assets/progressdialog.h
    Assets/progressdialog.ui
    Archives/zippackage.cpp
    Archives/zippackage.h
  )
endif()

include_directories(${CURL_INCLUDE_DIR})
include_directories(${LibArchive_INCLUDE_DIR})

target_link_libraries(ReleaseInstaller PRIVATE Qt5::Widgets Qt5::Network Qt5::Concurrent ${CURL_LIBRARIES} ${LibArchive_LIBRARIES})
