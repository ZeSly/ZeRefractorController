cmake_minimum_required (VERSION 3.0)

set(MAJOR_VERSION "1")
set(MINOR_VERSION "0")
set(PATCH_VERSION "0")

find_package(Qt5 COMPONENTS Widgets Network)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set (SRC
ascom_server.cpp
Comm.cpp
Device.cpp
DeviceData.cpp
firmwareupdate.cpp
hid_pnp.cpp
humiditygraph.cpp
ImportExportHex.cpp
main.cpp
temperaturegraph.cpp
weathergraph.cpp
zerefractorcontroller.cpp
)

set (HEADERS
DeviceData.h
Device.h
ImportExportHex.h
)

set (MOC_HEADERS
Comm.h
firmwareupdate.h
hid_pnp.h
temperaturegraph.h
weathergraph.h
zerefractorcontroller.h
humiditygraph.h
)

set (UIS
zerefractorcontroller.ui
FirmwareUpdate.ui
)

set(RESSOURCE zerefractorcontroller.qrc)

qt5_wrap_cpp(MOC_HDR ${MOC_HEADERS}) 
qt5_wrap_ui(UIS_HDR ${UIS})
qt5_add_resources(RESSOURCE_HDR ${RESSOURCE})

if (WIN32)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif(WIN32)

add_executable (${PROJECT_NAME} ${SRC} ${HEADERS} ${MOC_HDR} ${UIS_HDR} ${RESSOURCE_HDR})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Network hidapi udev)

install (TARGETS ${PROJECT_NAME} DESTINATION bin)

SET(CPACK_SET_DESTDIR "on")
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_DESCRIPTION "Ze Refractor Controller")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Ze Refractor Controller controls focus and dew heater on the refractor")
set(CPACK_PACKAGE_VENDOR "ZeSly")
set(CPACK_PACKAGE_CONTACT "zesly42@@gmail.com")
set(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.27), qt5-default (>= 5.5), libudev-dev")

include(CPack) 

