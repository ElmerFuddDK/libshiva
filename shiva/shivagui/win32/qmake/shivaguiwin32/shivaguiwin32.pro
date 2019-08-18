include(../../../../../shiva/include/shiva.pri)

TEMPLATE = lib
QT -= core gui qt
CONFIG -= qt

DESTDIR = $$_PRO_FILE_PWD_/../../../../$${SHVBUILDFOLDER}
QMAKE_LIBDIR += $$_PRO_FILE_PWD_/../../../../$${SHVBUILDFOLDER}

shivastaticlib {
  CONFIG += staticlib
}

LIBS += $$QMAKE_LIBS_DYNLOAD \
  -lshiva

include(../../../../../shiva/include/shiva-postconfig.pri)

INCLUDEPATH += ../../../../.. \
  .

DEF_FILE = $$_PRO_FILE_PWD_/exports.def

SOURCES += shvguiwin32.cpp \
  ../../../gui/shvcontrol.cpp \
  ../../../gui/shvcontrolbutton.cpp \
  ../../../gui/shvcontrolcheckbox.cpp \
  ../../../gui/shvcontrolcombobox.cpp \
  ../../../gui/shvcontrolcontainer.cpp \
  ../../../gui/shvcontroldatetime.cpp \
  ../../../gui/shvcontroledit.cpp \
  ../../../gui/shvcontrollabel.cpp \
  ../../../gui/shvcontrollistview.cpp \
  ../../../gui/shvcontrolprogressbar.cpp \
  ../../../gui/shvcontroltab.cpp \
  ../../../gui/shvformimpl.cpp \
  ../../../gui/shvguimanagerimpl.cpp \
  ../../../gui/shvmessagebox.cpp \
  ../../gui/shvwin32.cpp \
  ../../gui/shvmenuwin32.cpp \
  ../../gui/shvguimanagerwin32.cpp \
  ../../gui/shvcontrolwrapperwin32.cpp \
  ../../gui/shvcontrolcontainerwrapperwin32.cpp \
  ../../gui/shvmainthreadeventdispatcherwin32.cpp \
  ../../gui/shvcontrolimplementerbuttonwin32.cpp \
  ../../gui/shvcontrolimplementercomboboxwin32.cpp \
  ../../gui/shvcontrolimplementercontainerwindowwin32.cpp \
  ../../gui/shvcontrolimplementerdatetimewin32.cpp \
  ../../gui/shvcontrolimplementerdialogwin32.cpp \
  ../../gui/shvcontrolimplementereditwin32.cpp \
  ../../gui/shvcontrolimplementerlabelwin32.cpp \
  ../../gui/shvcontrolimplementerlistviewwin32.cpp \
  ../../gui/shvcontrolimplementercheckboxwin32.cpp \
  ../../gui/shvcontrolimplementermainwindowwin32.cpp \
  ../../gui/shvcontrolimplementerprogressbarwin32.cpp \
  ../../gui/shvcontrolimplementertabwin32.cpp \
  ../../gui/shvcontrolimplementerwin32base.cpp \
  ../../gui/utils/shvbitmapwin32.cpp \
  ../../gui/utils/shvbrushwin32.cpp \
  ../../gui/utils/shvcolorwin32.cpp \
  ../../gui/utils/shvdrawwin32.cpp \
  ../../gui/utils/shvfontwin32.cpp \
  ../../gui/utils/shvpenwin32.cpp \
  ../../../utilsimpl/shvcolor.cpp \
  ../../../utilsimpl/shvregionimpl.cpp
HEADERS += ../../gui/shvwin32.h \
  ../../gui/shvcontrolcontainerwrapperwin32.h \
  ../../gui/shvcontrolimplementerbuttonwin32.h \
  ../../gui/shvcontrolimplementercomboboxwin32.h \
  ../../gui/shvcontrolimplementerprogressbarwin32.h \
  ../../gui/shvcontrolimplementercontainerwindowwin32.h \
  ../../gui/shvcontrolimplementerdatetimewin32.h \
  ../../gui/shvcontrolimplementerdialogwin32.h \
  ../../gui/shvcontrolimplementereditwin32.h \
  ../../gui/shvcontrolimplementerlabelwin32.h \
  ../../gui/shvcontrolimplementerlistviewwin32.h \
  ../../gui/shvcontrolimplementercheckboxwin32.h \
  ../../gui/shvcontrolimplementermainwindowwin32.h \
  ../../gui/shvcontrolimplementertabwin32.h \
  ../../gui/shvcontrolimplementerwin32.h \
  ../../gui/shvcontrolimplementerwin32base.h \
  ../../gui/shvcontrolwrapperwin32.h \
  ../../gui/shvguimanagerwin32.h \
  ../../gui/shvmainthreadeventdispatcherwin32.h \
  ../../gui/shvmainwindowwin32.h \
  ../../gui/shvmenuwin32.h \
  ../../gui/utils/shvbitmapwin32.h \
  ../../gui/utils/shvbrushwin32.h \
  ../../gui/utils/shvcolorwin32.h \
  ../../gui/utils/shvdrawwin32.h \
  ../../gui/utils/shvfontwin32.h \
  ../../gui/utils/shvpenwin32.h \
  ../../../../include/framework/shveventstructs.h \
  ../../../../include/framework/shvmainthreadeventdispatcher.h \
  ../../../../include/utils/shvlist.h \
  ../../../../include/utils/shvptrcontainer.h \
  ../../../../include/utils/shvrefobject.h \
  ../../../../include/utils/shvvectorref.h \
  ../../../../include/gui/shvcompoundform.h \
  ../../../../include/gui/shvcontrol.h \
  ../../../../include/gui/shvcontrolbutton.h \
  ../../../../include/gui/shvcontrolcontainer.h \
  ../../../../include/gui/shvcontrolcreator.h \
  ../../../../include/gui/shvcontrolcreatorbase.h \
  ../../../../include/gui/shvcontrolcreatorsubtype.h \
  ../../../../include/gui/shvcontroldata.h \
  ../../../../include/gui/shvcontroldatetime.h \
  ../../../../include/gui/shvcontroledit.h \
  ../../../../include/gui/shvcontrolimplementer.h \
  ../../../../include/gui/shvcontrollabel.h \
  ../../../../include/gui/shvcontrollayout.h \
  ../../../../include/gui/shvcontrollistview.h \
  ../../../../include/gui/shvcontroltab.h \
  ../../../../include/gui/shvform.h \
  ../../../../include/gui/shvformbase.h \
  ../../../../include/gui/shvformimplementer.h \
  ../../../../include/gui/shvguimanager.h \
  ../../../../include/gui/shvmainwnd.h \
  ../../../../include/gui/shvmenu.h \
  ../../../../include/gui/shvregisterbitmap.h \
  ../../../../include/gui/utils/shvbitmap.h \
  ../../../../include/gui/utils/shvbrush.h \
  ../../../../include/gui/utils/shvcolor.h \
  ../../../../include/gui/utils/shvdraw.h \
  ../../../../include/gui/utils/shvdraweventdata.h \
  ../../../../include/gui/utils/shvfont.h \
  ../../../../include/gui/utils/shvpen.h \
  ../../../../include/gui/utils/shvpoint.h \
  ../../../../include/gui/utils/shvrect.h \
  ../../../../include/gui/utils/shvregion.h \
  ../../../../include/databinder/shvdatabinder.h

LIBS += -lcomctl32 -lgdi32
DEFINES += _WIN32_WINNT=0x0501 _WIN32_IE=0x0400
