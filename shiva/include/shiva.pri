
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
	DEFINES += SHIVASTATICMODULELIB
	SHVBUILDFOLDER = $${SHVBUILDFOLDER}-static
}

unix {
	include(./platforms/linux/shiva.pri)
}
