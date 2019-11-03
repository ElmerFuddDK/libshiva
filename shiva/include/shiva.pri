
QMAKE_CFLAGS_DEBUG *= -DDEBUG 
QMAKE_CXXFLAGS_DEBUG *= -DDEBUG 

CONFIG(debug, debug|release) {
	SHVBUILDFOLDER = build-debug
} else {
	SHVBUILDFOLDER = build
}

ios {
	CONFIG *= shivastaticlib
}

shivastaticlib {
	DEFINES += SHIVASTATICMODULELIB NOLIBSHIVA
	SHVBUILDFOLDER = $${SHVBUILDFOLDER}-static
}

shivanortti {
  QMAKE_CXXFLAGS += -fno-rtti
}

unix {
	include(./platforms/linux/shiva.pri)
}

win32-msvc* {
	QMAKE_CFLAGS_RELEASE ~= "s/-MD//"
	QMAKE_CFLAGS_RELEASE *= "-MT"
	QMAKE_CXXFLAGS_RELEASE ~= "s/-MD//"
	QMAKE_CXXFLAGS_RELEASE *= "-MT"
	DEFINES *= _WINDOWS
}

wince-*msvc* {
	QMAKE_CFLAGS_RELEASE ~= "s/-MD//"
	QMAKE_CFLAGS_RELEASE *= "-MT"
	QMAKE_CXXFLAGS_RELEASE ~= "s/-MD//"
	QMAKE_CXXFLAGS_RELEASE *= "-MT"
}
