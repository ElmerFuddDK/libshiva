#ifndef __SHIVA_GUI_CONTROLDATA_H
#define __SHIVA_GUI_CONTROLDATA_H

#include "../../include/utils/shvrefobject.h"
#include "../../include/utils/shvstring.h"
#include "utils/shvrect.h"


//-=========================================================================================================
/// SHVControlData - interface for data binding to controls
/**
 * Control data interface class.
 */

class SHVControlData : public SHVRefObject
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

	virtual SHVControlData* GetSubData(int entityIndex) = 0;
	virtual SHVControlData* GetSubData(SHVStringC entityName) = 0;

};
typedef SHVRefObjectContainer<SHVControlData> SHVControlDataRef;



#endif
