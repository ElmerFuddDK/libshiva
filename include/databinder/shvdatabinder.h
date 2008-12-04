#ifndef __SHIVA_DATABINDER_DATABINDER_H
#define __SHIVA_DATABINDER_DATABINDER_H

#include "../../include/utils/shvrefobject.h"
#include "../../include/utils/shvstring.h"


//-=========================================================================================================
/// SHVDataBinder - interface for data binding
/**
 * Data interface class.
 */

class SHVDataBinder : public SHVRefObject
{
public:

	virtual SHVBool SetPageSize(size_t pageSize) = 0;
	virtual size_t GetPageSize() = 0;

	virtual size_t GetRowCount() = 0;
	virtual size_t GetColCount() = 0;
	virtual SHVBool MoveNext() = 0;

	virtual SHVBool FindRowByIndex(size_t row = 0) = 0;
	virtual SHVBool FindRowByKey(const SHVStringC key) = 0;

	virtual SHVStringBuffer GetValue(size_t col = 0) = 0;
	virtual SHVBool SetValue(const SHVStringC value, size_t col = 0) = 0;

	virtual SHVStringBuffer GetValue(const SHVString8C col) = 0;
	virtual SHVBool SetValue(const SHVStringC value, const SHVString8C col) = 0;

	virtual SHVDataBinder* GetSubData(int entityIndex) = 0;
	virtual SHVDataBinder* GetSubData(SHVStringC entityName) = 0;

	virtual SHVBool PerformAction(const SHVChar* actionID) = 0;

};
typedef SHVRefObjectContainer<SHVDataBinder> SHVDataBinderRef;

#endif
