#-------------------------------------------------
#
# Project created by QtCreator 2010-06-29T15:14:45
#
#-------------------------------------------------

QT       -= core gui

LIBS += -L. \
        -lshiva \
        -lcomctl32 \
        -lgdi32
QMAKE_PRE_LINK = copydeps.bat

TEMPLATE = lib

SOURCES +=  ./shivaguiwin32.cpp \
    ../../gui/shvcontrol.cpp \
    ../../gui/shvcontrolbutton.cpp \
    ../../gui/shvcontrolcombobox.cpp \
    ../../gui/shvcontrolcontainer.cpp \
    ../../gui/shvcontroldatetime.cpp \
    ../../gui/shvcontroledit.cpp \
    ../../gui/shvcontrollabel.cpp \
    ../../gui/shvcontrollistview.cpp \
    ../../gui/shvcontroltab.cpp \
    ../../gui/shvformimpl.cpp \
    ../../gui/shvguimanagerimpl.cpp \
    ../../gui/shvmessagebox.cpp \
    ../gui/shvcontrolcontainerwrapperwin32.cpp \
    ../gui/shvcontrolimplementerbuttonwin32.cpp \
    ../gui/shvcontrolimplementercomboboxwin32.cpp \
    ../gui/shvcontrolimplementercontainerwindowwin32.cpp \
    ../gui/shvcontrolimplementerdatetimewin32.cpp \
    ../gui/shvcontrolimplementerdialogwin32.cpp \
    ../gui/shvcontrolimplementereditwin32.cpp \
    ../gui/shvcontrolimplementerlabelwin32.cpp \
    ../gui/shvcontrolimplementerlistviewwin32.cpp \
    ../gui/shvcontrolimplementermainwindowwin32.cpp \
    ../gui/shvcontrolimplementertabwin32.cpp \
    ../gui/shvcontrolimplementerwin32base.cpp \
    ../gui/shvcontrolwrapperwin32.cpp \
    ../gui/shvguimanagerwin32.cpp \
    ../gui/shvmainthreadeventdispatcherwin32.cpp \
    ../gui/shvmenuwin32.cpp \
    ../gui/shvwin32.cpp \
    ../gui/utils/shvbitmapwin32.cpp \
    ../gui/utils/shvbrushwin32.cpp \
    ../gui/utils/shvcolorwin32.cpp \
    ../gui/utils/shvdrawwin32.cpp \
    ../gui/utils/shvfontwin32.cpp \
    ../gui/utils/shvpenwin32.cpp \
    ../../utilsimpl/shvcolor.cpp \
    ../../utilsimpl/shvregionimpl.cpp


DEF_FILE = shivaguiwin32.def

QMAKE_CXXFLAGS_DEBUG += -DDEBUG 
