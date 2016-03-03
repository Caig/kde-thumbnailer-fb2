project(kde-thumbnailer-fb2)
cmake_minimum_required(VERSION 2.8.12)
set(QT_MIN_VERSION "5.2.0")

find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(FeatureSummary)
include(WriteBasicConfigVersionFile)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Gui)
find_package(KF5 REQUIRED COMPONENTS KIO Archive)

set(fb2thumbnail_SRCS fb2thumbnail.cpp fb2.cpp)

add_library(fb2thumbnail MODULE ${fb2thumbnail_SRCS})
target_link_libraries(fb2thumbnail Qt5::Gui KF5::KIOWidgets KF5::Archive)

install(TARGETS fb2thumbnail DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES fb2thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
