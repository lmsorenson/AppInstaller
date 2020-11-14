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
#
#set(prefix "${PROJECT_NAME}.app/Contents")
#set(INSTALL_RUNTIME_DIR "${prefix}/MacOS")
#set(INSTALL_CMAKE_DIR "${prefix}/Resources")

#set(DEST_DIR "/Users/Shared")
#
#macro(install_qt5_plugin _qt_plugin_name _qt_plugins_var _prefix)
#  get_target_property(_qt_plugin_path "${_qt_plugin_name}" LOCATION)
#  if(EXISTS "${_qt_plugin_path}")
#    get_filename_component(_qt_plugin_file "${_qt_plugin_path}" NAME)
#    get_filename_component(_qt_plugin_type "${_qt_plugin_path}" PATH)
#    get_filename_component(_qt_plugin_type "${_qt_plugin_type}" NAME)
#    set(_qt_plugin_dest "${_prefix}/PlugIns/${_qt_plugin_type}")
#    install(FILES "${_qt_plugin_path}"
#            DESTINATION "${_qt_plugin_dest}")
#    set(${_qt_plugins_var}
#            "${${_qt_plugins_var}};${DEST_DIR}${CMAKE_INSTALL_PREFIX}/${_qt_plugin_dest}/${_qt_plugin_file}")
#  else()
#    message(FATAL_ERROR "QT plugin ${_qt_plugin_name} not found")
#  endif()
#endmacro()
#
#install_qt5_plugin("Qt5::QCocoaIntegrationPlugin" QT_PLUGINS ${prefix})
#install_qt5_plugin("Qt5::Widgets" QT_PLUGINS ${prefix})
#install_qt5_plugin("Qt5::Concurrent" QT_PLUGINS ${prefix})
#install_qt5_plugin("Qt5::Network" QT_PLUGINS ${prefix})
#file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
#        "[Paths]\nPlugins = ${_qt_plugin_dir}\n")
#install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
#        DESTINATION "${INSTALL_CMAKE_DIR}")
#
## Destination paths below are relative to ${CMAKE_INSTALL_PREFIX}
#install(TARGETS ${PROJECT_NAME}
#        BUNDLE DESTINATION . COMPONENT Runtime
#        RUNTIME DESTINATION ${INSTALL_RUNTIME_DIR} COMPONENT Runtime
#        )
#
## Note Mac specific extension .app
#set(APPS "${DEST_DIR}${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app")
#
## Directories to look for dependencies
#set(DIRS "${CMAKE_BINARY_DIR}")
#
## Path used for searching by FIND_XXX(), with appropriate suffixes added
#if(CMAKE_PREFIX_PATH)
#  foreach(dir ${CMAKE_PREFIX_PATH})
#    list(APPEND DIRS "${dir}/bin" "${dir}/lib")
#  endforeach()
#endif()
#
## Append Qt's lib folder which is two levels above Qt5Widgets_DIR
#list(APPEND DIRS "${Qt5Widgets_DIR}/../..")
#
#include(InstallRequiredSystemLibraries)
#
#message(STATUS "APPS: ${APPS}")
#message(STATUS "QT_PLUGINS: ${QT_PLUGINS}")
#message(STATUS "DIRS: ${DIRS}")
#
#install(CODE "include(BundleUtilities)
#    fixup_bundle(\"${APPS}\" \"${QT_PLUGINS}\" \"${DIRS}\")")
#
#set(CPACK_GENERATOR "DRAGNDROP")
#include(CPack)

