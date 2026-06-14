QT += core gui testlib widgets sql 3dcore 3drender 3dextras 3dinput 3dlogic

CONFIG += console testcase c++17
CONFIG -= app_bundle
CONFIG += testcase

SOURCES += \
    tst_all.cpp \
    ../auth/login.cpp \
    ../auth/registerpage.cpp \
    ../core/controller.cpp \
    ../core/filterengine.cpp \
    ../database/favoriteRepository.cpp \
    ../database/starrepository.cpp \
    ../utils/csvimporter.cpp \
    ../view/widgets/filterpanel.cpp \
    ../view/3d/star3dview.cpp \
    ../view/3d/starentity.cpp \
    ../view/widgets/searchpanel.cpp


HEADERS += \
    ../auth/login.h \
    ../auth/registerpage.h \
    ../controller.h \
    ../core/filterengine.h \
    ../database/favoriteRepository.h \
    ../database/starrepository.h \
    ../models/StarFilter.h \
    ../utils/csvimporter.h \
    ../view/widgets/filterpanel.h \
    ../view/3d/star3dview.h \
    ../view/3d/starentity.h \
    ../view/3d/starobject.h \
    ../view/widgets/searchpanel.h

INCLUDEPATH += \
    ../auth \
    ../database \
    ../core \
    ../view \
    ../utils \
    ../models \
    ..

FORMS += \
    ../auth/login.ui \
    ../auth/registerpage.ui \
    ../auth/login.ui
