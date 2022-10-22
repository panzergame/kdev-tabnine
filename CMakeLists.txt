cmake_minimum_required(VERSION 3.16)

project(kdevtabnine)

cmake_policy(SET CMP0135 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

find_package (ECM 5.15.0 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(FetchContent)
include(FeatureSummary)

set(QT_MIN_VERSION "5.5.0")
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Core Widgets Test)
set(KF5_DEP_VERSION "5.15.0")
find_package(KF5 ${KF5_DEP_VERSION} REQUIRED COMPONENTS ItemModels TextEditor ThreadWeaver)
set(KDEVPLATFORM_VERSION "5.0.0")
find_package(KDevPlatform ${KDEVPLATFORM_VERSION} CONFIG)
set_package_properties(KDevPlatform PROPERTIES
    TYPE REQUIRED
)

add_definitions(-DTRANSLATION_DOMAIN=\"kdevtabnine\")

set(kdevtabnine_PART_SRCS
    model.cpp
    resultentry.cpp
    service.cpp
    tabnineplugin.cpp
    truncablestring.cpp
)

ecm_qt_declare_logging_category(kdevtabnine_PART_SRCS
    HEADER debug.h
    IDENTIFIER KDEV_TABNINE
    CATEGORY_NAME "kdevelop.plugins.tabnine"
    DESCRIPTION "KDevelop plugin: Tabnine completion"
    EXPORT KDEVTABNINE
)

kdevplatform_add_plugin(kdevtabnine JSON kdevtabnine.json SOURCES ${kdevtabnine_PART_SRCS})
target_link_libraries(kdevtabnine
    KDev::Interfaces
    KDev::Language
)

set(TABNINE_VERSION "4.4.153" CACHE STRING "TabNine version to download")

if (UNIX)
    set(TABNINE_PLATFORM "x86_64-unknown-linux-musl")
elseif (WIN32)
    set(TABNINE_PLATFORM "x86_64-pc-windows-gnu")
endif()


set(TABNINE_URL "https://update.tabnine.com/bundles/${TABNINE_VERSION}/${TABNINE_PLATFORM}/TabNine.zip")

FetchContent_Declare(
  tabnine
  URL ${TABNINE_URL}
)

FetchContent_MakeAvailable(tabnine)

install(PROGRAMS ${tabnine_SOURCE_DIR}/TabNine DESTINATION share/${PROJECT_NAME})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
