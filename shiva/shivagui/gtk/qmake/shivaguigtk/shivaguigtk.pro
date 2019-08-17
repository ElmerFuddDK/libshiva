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

INCLUDEPATH += ../../../../..

DEF_FILE = $$_PRO_FILE_PWD_/exports.def

SOURCES += shvguigtk.cpp \
  ../../../gui/shvcontrolcontainer.cpp \
  ../../../gui/shvcontrol.cpp \
  ../../../gui/shvcontrolcombobox.cpp \
  ../../../gui/shvcontrollistview.cpp \
  ../../../gui/shvcontrolcheckbox.cpp \
  ../../../gui/shvcontroltab.cpp \
  ../../../gui/shvcontroledit.cpp \
  ../../../gui/shvcontrollabel.cpp \
  ../../../gui/shvformimpl.cpp \
  ../../../gui/shvcontrolprogressbar.cpp \
  ../../../gui/shvguimanagerimpl.cpp \
  ../../../gui/shvmessagebox.cpp \
  ../../../gui/shvcontrolbutton.cpp \
  ../../../utilsimpl/shvregionimpl.cpp \
  ../../../utilsimpl/shvcolor.cpp \
  ../../gui/utils/shvfontgtk.cpp \
  ../../gui/utils/shvcolorgtk.cpp \
  ../../gui/utils/shvdrawgtk.cpp \
  ../../gui/utils/shvpengtk.cpp \
  ../../gui/utils/shvbitmapgtk.cpp \
  ../../gui/shvgtk.cpp \
  ../../gui/shvmenugtkcontainer.cpp \
  ../../gui/shvmenugtksub.cpp \
  ../../gui/shvmenugtkpopup.cpp \
  ../../gui/shvmainthreadeventdispatchergtk.cpp \
  ../../gui/shvguimanagergtk.cpp \
  ../../gui/shvcontrolimplementermainwindowgtk.cpp \
  ../../gui/shvcontrolimplementerlabelgtk.cpp \
  ../../gui/shvcontrolimplementergtkwidgetbase.cpp \
  ../../gui/shvcontrolimplementerbuttongtk.cpp \
  ../../gui/shvcontrolimplementerdialoggtk.cpp \
  ../../gui/shvcontrolimplementereditgtk.cpp \
  ../../gui/shvcontrolimplementereditmultigtk.cpp \
  ../../gui/shvcontrolimplementercontainerwindowgtk.cpp \
  ../../gui/shvcontrolimplementerprogressbargtk.cpp \
  ../../gui/shvcontrolimplementercomboboxgtk.cpp \
  ../../gui/shvcontrolimplementerlistviewgtk.cpp \
  ../../gui/shvcontrolimplementercheckboxgtk.cpp \
  ../../gui/shvcontrolimplementertabgtk.cpp
HEADERS += ../../../gui/shvformimpl.h \
  ../../../gui/shvguimanagerimpl.h \
  ../../../gui/shvmessagebox.h \
  ../../../utilsimpl/shvregionimpl.h \
  ../../gui/shvmainthreadeventdispatchergtk.h \
  ../../gui/shvgtk.h \
  ../../gui/shvguimanagergtk.h \
  ../../gui/shvcontrolimplementermainwindowgtk.h \
  ../../../../include/gui/shvcompoundform.h \
  ../../../../include/gui/shvcontrolbutton.h \
  ../../../../include/gui/shvcontrolcombobox.h \
  ../../../../include/gui/shvcontrolcontainer.h \
  ../../../../include/gui/shvcontrolcreatorbase.h \
  ../../../../include/gui/shvcontrolcreator.h \
  ../../../../include/gui/shvcontrolcreatorsubtype.h \
  ../../../../include/gui/shvcontroldatetime.h \
  ../../../../include/gui/shvcontroledit.h \
  ../../../../include/gui/shvcontrol.h \
  ../../../../include/gui/shvcontrolimplementer.h \
  ../../../../include/gui/shvcontrollabel.h \
  ../../../../include/gui/shvcontrollayout.h \
  ../../../../include/gui/shvcontrollistview.h \
  ../../../../include/gui/shvcontrolcheckbox.h \
  ../../../../include/gui/shvcontrolprogressbar.h \
  ../../../../include/gui/shvcontroltab.h \
  ../../../../include/gui/shvformbase.h \
  ../../../../include/gui/shvform.h \
  ../../../../include/gui/shvformimplementer.h \
  ../../../../include/gui/shvguimanager.h \
  ../../../../include/gui/shvmainwnd.h \
  ../../../../include/gui/shvmenu.h \
  ../../../../include/gui/shvregisterbitmap.h \
  ../../../../include/gui/utils/shvbitmap.h \
  ../../../../include/gui/utils/shvbrush.h \
  ../../../../include/gui/utils/shvcolor.h \
  ../../../../include/gui/utils/shvdraweventdata.h \
  ../../../../include/gui/utils/shvdraw.h \
  ../../../../include/gui/utils/shvfont.h \
  ../../../../include/gui/utils/shvpen.h \
  ../../../../include/gui/utils/shvpoint.h \
  ../../../../include/gui/utils/shvrect.h \
  ../../../../include/gui/utils/shvregion.h \
  ../../gui/utils/shvfontgtk.h \
  ../../gui/utils/shvcolorgtk.h \
  ../../gui/shvcontrolimplementerlabelgtk.h \
  ../../gui/shvcontrolimplementergtkwidget.h \
  ../../gui/shvcontrolimplementergtkwidgetbase.h \
  ../../gui/utils/shvdrawgtk.h \
  ../../gui/utils/shvpengtk.h \
  ../../gui/utils/shvbitmapgtk.h \
  ../../gui/shvcontrolimplementerbuttongtk.h \
  ../../gui/shvcontrolimplementerdialoggtk.h \
  ../../gui/shvcontrolimplementereditgtk.h \
  ../../gui/shvcontrolimplementereditmultigtk.h \
  ../../gui/shvcontrolimplementercontainerwindowgtk.h \
  ../../gui/shvcontrolimplementerprogressbargtk.h \
  ../../gui/shvmenugtkcontainer.h \
  ../../gui/shvmenugtksub.h \
  ../../gui/shvmenugtkroot.h \
  ../../gui/shvmenugtkpopup.h \
  ../../gui/shvcontrolimplementercomboboxgtk.h \
  ../../gui/shvcontrolimplementerlistviewgtk.h \
  ../../gui/shvcontrolimplementercheckboxgtk.h \
  ../../gui/shvcontrolimplementertabgtk.h


LIBS += -lgdk-x11-2.0 -lgtk-x11-2.0

DEFINES += __SHIVA_GTK

unix {
  CONFIG += link_pkgconfig
  PKGCONFIG += gtk+-x11-2.0
}
