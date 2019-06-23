project(maui-accounts-gui LANGUAGES CXX)

cmake_minimum_required(VERSION 3.10)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM 1.7.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)
include(FeatureSummary)

find_package(
    Qt5 REQUIRED NO_MODULE

    COMPONENTS
        Qml
        Concurrent
        Svg
        Quick
        Widgets
        DBus
)
include(FeatureSummary)

find_package(libdavclient REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTORCC ON)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr CACHE PATH "" FORCE)
endif()

### maui-accounts-gui
set(
    ACCOUNTS_SRCS
    
    src/main.cpp
    src/entities/SyncManager.cpp
    src/entities/LocalContacts.cpp
    src/viewcontrollers/MainViewController.cpp
)

set(
    ACCOUNTS_ASSETS
    src/qml/qml.qrc
)

add_executable(
    maui-accounts-gui

    ${ACCOUNTS_SRCS}
    ${ACCOUNTS_ASSETS}
)
target_link_libraries(
    maui-accounts-gui

    Qt5::Qml
    Qt5::Quick
    Qt5::Widgets
    Qt5::Concurrent
    Qt5::Svg
    Qt5::DBus

    libdavclient.so
)

install(TARGETS maui-accounts-gui DESTINATION bin)
### END maui-accounts-gui



# CPack Configuration
include(InstallRequiredSystemLibraries)

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Anupam Basak")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MAUI Accounts GUI")
set(CPACK_PACKAGE_VENDOR "Anupam Basak")
set(CPACK_PACKAGE_CONTACT "Anupam Basak <anupam.basak27@gmail.com>")
#set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
set(CPACK_SOURCE_STRIP_FILES "")

include(CPack)
###

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
