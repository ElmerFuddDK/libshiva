
*-g++* {
	exists( $$PWD/glibcsyms.h ) {
		QMAKE_CFLAGS_RELEASE ~= "s/.*D_FORTIFY_SOURCE.*/"
		QMAKE_CXXFLAGS_RELEASE~="s/.*D_FORTIFY_SOURCE.*/"
		QMAKE_CFLAGS_RELEASE += -U_FORTIFY_SOURCE -include "$$PWD/glibcsyms.h"
		QMAKE_CXXFLAGS_RELEASE += -U_FORTIFY_SOURCE -include "$$PWD/glibcsyms.h"
	}
}
