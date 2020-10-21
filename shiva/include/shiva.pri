
QMAKE_CFLAGS_DEBUG *= -DDEBUG 
QMAKE_CXXFLAGS_DEBUG *= -DDEBUG 

CONFIG(debug, debug|release) {
	isEmpty(SHVBUILDFOLDERPREFIX) {
		SHVBUILDFOLDER = build-debug
	} else {
		SHVBUILDFOLDER = build-$${SHVBUILDFOLDERPREFIX}-debug
	}
} else {
	isEmpty(SHVBUILDFOLDERPREFIX) {
		SHVBUILDFOLDER = build
	} else {
		SHVBUILDFOLDER = build-$${SHVBUILDFOLDERPREFIX}
	}
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
	
	shivanodebugdll {
		QMAKE_CFLAGS_DEBUG ~= "s/-MDd//"
		QMAKE_CFLAGS_DEBUG *= "-MTd"
		QMAKE_CXXFLAGS_DEBUG ~= "s/-MDd//"
		QMAKE_CXXFLAGS_DEBUG *= "-MTd"
	}
}

wince-*msvc* {
	QMAKE_CFLAGS_RELEASE ~= "s/-MD//"
	QMAKE_CFLAGS_RELEASE *= "-MT"
	QMAKE_CXXFLAGS_RELEASE ~= "s/-MD//"
	QMAKE_CXXFLAGS_RELEASE *= "-MT"
	
	shivanodebugdll {
		QMAKE_CFLAGS_DEBUG ~= "s/-MDd//"
		QMAKE_CFLAGS_DEBUG *= "-MTd"
		QMAKE_CXXFLAGS_DEBUG ~= "s/-MDd//"
		QMAKE_CXXFLAGS_DEBUG *= "-MTd"
	}
}
