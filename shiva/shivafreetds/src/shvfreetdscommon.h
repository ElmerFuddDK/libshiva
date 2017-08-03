#ifndef __SHIVA_FREETDS_COMMON_H
#define __SHIVA_FREETDS_COMMON_H

///\cond INTERNAL

#ifdef INLINE_FREETDS
# include <freetds/tds.h>
# include <freetds/iconv.h>
# include <freetds/string.h>
# include <freetds/convert.h>
# include <freetds/data.h>
# include <freetds/utils.h>
#else
# include <sybdb.h>
# ifndef SYBUNIQUE
#  define SYBUNIQUE 36 // Missing uuid support
#  define SYBSHVFAKEUNIQUE
# endif
#endif

#include "shiva/include/utils/shvrefobject.h"

class SHVFreeTDSLogin : public SHVRefObject
{
public:
	LOGINREC* Login;
	inline SHVFreeTDSLogin(LOGINREC* login) : Login(login) {}
	inline ~SHVFreeTDSLogin() { if (Login) dbloginfree(Login); }
};
typedef SHVRefObjectContainer<SHVFreeTDSLogin> SHVFreeTDSLoginRef;

///\endcond

#endif
