#-------------------------------------------------
#
# Project created by QtCreator 2018-07-05T22:26:31
#
#-------------------------------------------------

QT       += core gui network widgets


# For OpenSSL 1.1.1, we need Qt 5.12.4+.
#   https://www.qt.io/blog/2019/06/17/qt-5-12-4-released-support-openssl-1-1-1
#   https://github.com/BesLyric-for-X/BesLyric-for-X/issues/29

is_qt_version_too_low = false

lessThan(QT_MAJOR_VERSION, 5) {
    is_qt_version_too_low = true
}
else:equals(QT_MAJOR_VERSION, 5) {
    lessThan(QT_MINOR_VERSION, 12) {
        is_qt_version_too_low = true
    }
    else:equals(QT_MINOR_VERSION, 12):lessThan(QT_PATCH_VERSION, 4) {
        is_qt_version_too_low = true
    }
}

equals(is_qt_version_too_low, true) {
    error("Qt version >= 5.12.4 is required, you are using $$[QT_VERSION].")
}


TARGET = BesLyric-for-X
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# To enable qDebug() under "release", disable QT_NO_DEBUG_OUTPUT bellow
CONFIG (release, debug|release) {
DEFINES *= QT_NO_DEBUG_OUTPUT
}

INCLUDEPATH +=$$PWD BesWidgets
include(BesWidgets/BesWidgets.pri)

INCLUDEPATH +=$$PWD StackFrame
include(StackFrame/StackFrame.pri)

INCLUDEPATH +=$$PWD TopWidgets
include(TopWidgets/TopWidgets.pri)

INCLUDEPATH +=$$PWD MiddleWidgets
include(MiddleWidgets/MiddleWidgets.pri)

INCLUDEPATH +=$$PWD BottomWidgets
include(BottomWidgets/BottomWidgets.pri)

INCLUDEPATH +=$$PWD Entities
include(Entities/Entities.pri)

INCLUDEPATH +=$$PWD Utility
include(Utility/Utility.pri)


SOURCES += main.cpp \
    StackFrame.cpp

HEADERS  += \
    Define/Define.h\
    Define/Enum.h\
    Define/Struct.h\
    Define/Static.h\
    global.h \
    StackFrame.h \
    MyApplication.h

DISTFILES += \
    BesLyric.rc \
    version.txt

RESOURCES += \
    resource.qrc


# windows icon and exe file infomation
win32{
RC_FILE = Beslyric.rc
}

# set icon under Mac Os
macx{
ICON = BesLyric.icns
}

# ubuntu icon recoginition
#  No test on other Linux distros!
# from: https://stackoverflow.com/questions/45329372/ubuntu-recognizes-executable-as-shared-library-and-wont-run-it-by-clicking
!macx:unix{
    QMAKE_LFLAGS *= -no-pie
}

#--------------------------------

# Separate the binary file.
CONFIG(debug, debug|release){
    DESTDIR = $${OUT_PWD}/debug_bin
}
CONFIG(release, debug|release){
    DESTDIR = $${OUT_PWD}/release_bin
}

#--------------------------------

#屏蔽 msvc 编译器对 rational.h 的 warning: C4819: 该文件包含不能在当前代码页(936)中表示的字符。请将该文件保存为 Unicode 格式以防止数据丢失
win32-msvc*:QMAKE_CXXFLAGS += /wd"4819"

#--------------------------------

# Third-party libraries

win32 {
    isEmpty(B4X_DEP_PATH) {
        B4X_DEP_PATH = $$getenv(B4X_DEP_PATH)
    }
    isEmpty(B4X_DEP_PATH) {
        error("\"B4X_DEP_PATH\" is NOT set.")
    }
    contains(B4X_DEP_PATH, "^\s*$") {
        error("\"B4X_DEP_PATH\" is empty.")
    }
    message("B4X_DEP_PATH = $${B4X_DEP_PATH}")

    dep_base_path = $$system_path($$absolute_path($$clean_path($${B4X_DEP_PATH})))
    !exists($${dep_base_path}) {
        error("\"$${dep_base_path}\" does NOT exist.")
    }
    message("dep_base_path = $${dep_base_path}")

    dep_include_path = $$system_path($${dep_base_path}/include)
    !exists($${dep_include_path}) {
        error("\"$${dep_include_path}\" does NOT exist.")
    }
    message("dep_include_path = $${dep_include_path}")

    dep_lib_path = $$system_path($${dep_base_path}/lib)
    !exists($${dep_lib_path}) {
        error("\"$${dep_lib_path}\" does NOT exist.")
    }
    message("dep_lib_path = $${dep_lib_path}")

    dep_bin_path = $$system_path($${dep_base_path}/bin)
    !exists($${dep_bin_path}) {
        error("\"$${dep_bin_path}\" does NOT exist.")
    }
    message("dep_bin_path = $${dep_bin_path}")

    INCLUDEPATH *= \
        $${dep_include_path} \
        $$system_path($${dep_include_path}/SDL2)
    LIBS *= \
        -L$${dep_lib_path} \
        -lavcodec \
        -lavdevice \
        -lavfilter \
        -lavformat \
        -lavutil \
        -lpostproc \
        -lswresample \
        -lswscale \
        -lSDL2

    unset(dep_bin_path)
    unset(dep_lib_path)
    unset(dep_include_path)
    unset(dep_base_path)
    unset(B4X_DEP_PATH)
}

unix {
    CONFIG *= link_pkgconfig
    PKGCONFIG *= \
        libavcodec \
        libavdevice \
        libavfilter \
        libavformat \
        libavutil \
        libpostproc \
        libswresample \
        libswscale \
        sdl2
}

#--------------------------------
