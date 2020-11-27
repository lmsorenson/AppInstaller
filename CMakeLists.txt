cmake_minimum_required(VERSION 3.10)

project(ReleaseInstaller LANGUAGES CXX VERSION $ENV{DEPLOY_VERSION})
message("DEPLOY VERSION: $ENV{DEPLOY_VERSION}")

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

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS Concurrent REQUIRED)
find_package(LibArchive)
find_package(CURL REQUIRED)

if(ANDROID)
  add_library(ReleaseInstaller SHARED
    src/main.cpp
    src/Assets/assetmanagerbase.cpp
    src/Assets/assetmanagerbase.h
    src/Assets/github/githubassetmanager.cpp
    src/Assets/github/githubassetmanager.h
    src/Assets/download.h
    src/Assets/download.cpp
    src/Archives/zippackage.cpp
    src/Archives/zippackage.h
    src/UserInterfacemainwindow.cpp
    src/UserInterface/mainwindow.h
    src/UserInterface/mainwindow.ui
    src/UserInterface/progressdialog.cpp
    src/UserInterface/progressdialog.h
    src/UserInterface/progressdialog.ui
    src/UserInterface/package.cpp
    src/UserInterface/Package.h
  )
else()
  add_executable(ReleaseInstaller MACOSX_BUNDLE
    src/main.cpp
    src/Assets/assetmanagerbase.cpp
    src/Assets/assetmanagerbase.h
    src/Assets/github/githubassetmanager.cpp
    src/Assets/github/githubassetmanager.h
    src/Assets/download.h
    src/Assets/download.cpp
    src/Archives/zippackage.cpp
    src/Archives/zippackage.h
    src/UserInterface/mainwindow.cpp
    src/UserInterface/mainwindow.h
    src/UserInterface/mainwindow.ui
    src/UserInterface/progressdialog.cpp
    src/UserInterface/progressdialog.h
    src/UserInterface/progressdialog.ui
    src/UserInterface/package.cpp
    src/UserInterface/Package.h
  )
endif()

include_directories(${CURL_INCLUDE_DIR})
include_directories(${LibArchive_INCLUDE_DIR})

target_link_libraries(ReleaseInstaller PRIVATE Qt5::Widgets Qt5::Network Qt5::Concurrent ${CURL_LIBRARIES} ${LibArchive_LIBRARIES})

set(prefix "${PROJECT_NAME}.app/Contents")
set(INSTALL_RUNTIME_DIR "${prefix}/MacOS")
set(INSTALL_CMAKE_DIR "${prefix}/Resources")

set(CPACK_BUNDLE_NAME "ReleaseInstaller")

macro(install_qt5_plugin _qt_plugin_name _qt_plugins_var _prefix)
  get_target_property(_qt_plugin_path "${_qt_plugin_name}" LOCATION)
  message("checking if plugin path exists")
  if(EXISTS "${_qt_plugin_path}")
    get_filename_component(_qt_plugin_file "${_qt_plugin_path}" NAME)
    get_filename_component(_qt_plugin_type "${_qt_plugin_path}" PATH)
    get_filename_component(_qt_plugin_type "${_qt_plugin_type}" NAME)
    set(_qt_plugin_dest "${_prefix}/PlugIns/${_qt_plugin_type}")
    message("${_qt_plugin_path} to ${_qt_plugin_dest}")
    install(FILES "${_qt_plugin_path}"
            DESTINATION "${_qt_plugin_dest}")
    set(${_qt_plugins_var}
            "${${_qt_plugins_var}};\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${_qt_plugin_dest}/${_qt_plugin_file}")
  else()
    message(FATAL_ERROR "QT plugin ${_qt_plugin_name} not found")
  endif()
endmacro()

install_qt5_plugin("Qt5::QCocoaIntegrationPlugin" QT_PLUGINS ${prefix})
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
        "[Paths]\nPlugins = ${_qt_plugin_dir}\n")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
        DESTINATION "${INSTALL_CMAKE_DIR}")

message ("${CMAKE_INSTALL_PREFIX}")

# Destination paths below are relative to ${CMAKE_INSTALL_PREFIX}
install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION . COMPONENT Runtime
        RUNTIME DESTINATION ${INSTALL_RUNTIME_DIR} COMPONENT Runtime)

# Note Mac specific extension .app
set(APPS "\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app")

# Directories to look for dependencies
set(DIRS "${CMAKE_BINARY_DIR}")

message(STATUS "CMAKE BINARY: ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE PREFIX PATH: ${CMAKE_PREFIX_PATH}")

# Path used for searching by FIND_XXX(), with appropriate suffixes added
if(CMAKE_PREFIX_PATH)
  foreach(dir ${CMAKE_PREFIX_PATH})
    message(STATUS "${dir}")
    list(APPEND DIRS "${dir}/bin" "${dir}/lib")
  endforeach()
endif()

# Append Qt's lib folder which is two levels above Qt5Widgets_DIR
list(APPEND DIRS "${Qt5Widgets_DIR}/../..")

include(InstallRequiredSystemLibraries)

message(STATUS "DESTDIR: $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}")
message(STATUS "APPS: ${APPS}")
message(STATUS "QT_PLUGINS: ${QT_PLUGINS}")
message(STATUS "DIRS: ${DIRS}")

install(CODE "include(BundleUtilities)
    fixup_bundle(\"${APPS}\" \"${QT_PLUGINS}\" \"${DIRS}\")")

set(CPACK_GENERATOR "DRAGNDROP")
include(CPack)
