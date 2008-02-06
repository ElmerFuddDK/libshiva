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

class SHVControlDataRow : public SHVRefObject
{
public:

	virtual SHVStringBuffer GetValue(int col = 0) = 0;
	virtual SHVBool SetValue(const SHVStringC value, int col = 0) = 0;

	virtual SHVStringBuffer GetValue(const SHVStringC col) = 0;
	virtual SHVBool SetValue(const SHVStringC value, const SHVStringC col) = 0;
};
typedef SHVRefObjectContainer<SHVControlDataRow> SHVControlDataRowRef;


class SHVControlDataIterator : public SHVRefObject
{
public:


	virtual SHVBool SetNext() = 0;

	virtual SHVControlDataRow* GetRow() = 0;

};


class SHVControlData : public SHVRefObject
{
public:

	virtual SHVControlDataRow* GetRow(int row = 0) = 0;
	virtual SHVControlDataRow* GetRowByKey(const SHVStringC key) = 0;

	virtual SHVControlDataIterator* GetIterator() = 0;

};
typedef SHVRefObjectContainer<SHVControlDataRow> SHVControlDataRowRef;



#endif
