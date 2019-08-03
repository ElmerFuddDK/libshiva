
QMAKE_CFLAGS_DEBUG *= -DDEBUG 
QMAKE_CXXFLAGS_DEBUG *= -DDEBUG 

ios {
	CONFIG *= shivastaticlib
}

shivastaticlib {
	DEFINES += SHIVASTATICMODULELIB
}

unix {
	include(./platforms/linux/shiva.pri)
}
