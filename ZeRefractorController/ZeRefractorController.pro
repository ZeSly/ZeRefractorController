# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

TEMPLATE = app
TARGET = ZeRefractorController
DESTDIR = ../Debug
QT += core network widgets gui
CONFIG += debug
DEFINES += QT_LARGEFILE_SUPPORT QT_NETWORK_LIB QT_WIDGETS_LIB
INCLUDEPATH += ./GeneratedFiles \
    ./GeneratedFiles/Debug \
    .
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/debug
OBJECTS_DIR += debug
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(ZeRefractorController.pri)
TRANSLATIONS += zerefractorcontroller_en.ts \
    zerefractorcontroller_fr.ts
win32:RC_FILE = ZeRefractorController.rc
