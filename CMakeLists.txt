project(kde-thumbnailer-fb2)

cmake_minimum_required(VERSION 2.6)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${QT_INCLUDES} ${KDE4_INCLUDES})

set(fb2thumbnail_SRCS fb2thumbnail.cpp)

kde4_add_plugin(fb2thumbnail ${fb2thumbnail_SRCS})
target_link_libraries(fb2thumbnail ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY} ${KDE4_KIO_LIBS})

install(TARGETS fb2thumbnail DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES fb2thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})