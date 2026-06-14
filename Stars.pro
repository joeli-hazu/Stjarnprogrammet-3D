QT       += core gui
QT       += 3dcore 3drender 3dinput 3dextras
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
QMAKE_CXXFLAGS += -Wno-implicit-function-declaration


INCLUDEPATH += . \
    auth \
    core \
    database \
    models \
    utils \
    view/3d \
    view/widgets

HEADERS += \
    auth/login.h \
    auth/registerpage.h \
    core/controller.h \
    core/filterengine.h \
    database/databasemanager.h \
    database/starrepository.h \
    models/Star.h \
    models/StarFilter.h \
    models/User.h \
    utils/CsvImporter.h \
    utils/StarColorHelper.h \
    view/3d/star3dview.h \
    view/3d/starentity.h \
    view/3d/starobject.h \
    view/widgets/filterpanel.h \
    view/widgets/mainwindow.h \
    view/widgets/adminpanel.h \
    view/widgets/searchpanel.h

SOURCES += \
    main.cpp \
    auth/login.cpp \
    auth/registerpage.cpp \
    core/controller.cpp \
    core/filterengine.cpp \
    database/databasemanager.cpp \
    database/favoriteRepository.cpp \
    database/starrepository.cpp \
    utils/CsvImporter.cpp \
    view/3d/star3dview.cpp \
    view/3d/starentity.cpp \
    view/widgets/adminpanel.cpp \
    view/widgets/filterpanel.cpp \
    view/widgets/mainwindow.cpp \
    view/widgets/mainwindow_logik.cpp \
    view/widgets/mainwindow_ui.cpp \
    view/widgets/searchpanel.cpp




FORMS += \
    view/widgets/mainwindow.ui \
    auth/login.ui \
    auth/RegisterPage.ui

RESOURCES += \
    textures/resources.qrc
