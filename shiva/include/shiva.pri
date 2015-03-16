
QMAKE_CFLAGS_DEBUG *= -DDEBUG 
QMAKE_CXXFLAGS_DEBUG *= -DDEBUG 

unix {
	include(./platforms/linux/shiva.pri)
}