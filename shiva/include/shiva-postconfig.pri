!isEmpty(ANDROID_PLATFORM){
  QMAKE_LIBS *= -llog
  LIBS -= -lrt \
          -lpthread \
          -lgnustl_shared
  QMAKE_CFLAGS += -fno-exceptions
  QMAKE_CXXFLAGS += -fno-exceptions -fno-rtti
  QMAKE_LIBS -= -lgnustl_static -lgnustl_shared
  QMAKE_LIBS_PRIVATE -= -lgnustl_static -lgnustl_shared
  QMAKE_INCDIR -= $$ANDROID_SOURCES_CXX_STL_INCDIR
  QMAKE_LIBDIR -= $$ANDROID_SOURCES_CXX_STL_LIBDIR
}
