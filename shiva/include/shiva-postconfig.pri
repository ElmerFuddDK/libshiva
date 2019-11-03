!isEmpty(ANDROID_PLATFORM){
  QMAKE_LIBS *= -llog
  shivaandroidstl {
    LIBS -= -lrt \
            -lpthread
  } else {
    LIBS -= -lrt \
            -lpthread \
            -lgnustl_shared
    QMAKE_LIBS -= -lgnustl_static -lgnustl_shared
    QMAKE_LIBS_PRIVATE -= -lgnustl_static -lgnustl_shared
    shivanortti {
      QMAKE_INCDIR -= $$ANDROID_SOURCES_CXX_STL_INCDIR
      QMAKE_LIBDIR -= $$ANDROID_SOURCES_CXX_STL_LIBDIR
    }
  }
  QMAKE_CFLAGS += -fno-exceptions
  QMAKE_CXXFLAGS += -fno-exceptions
  !shivanortti {
    QMAKE_LIBS *= -lgnustl_static
  }
}
