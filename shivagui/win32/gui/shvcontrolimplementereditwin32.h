#ifndef __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTEREDIT_H
#define __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTEREDIT_H


#include "../../../include/gui/shvcontroledit.h"
#include "shvcontrolimplementerwin32.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif


//-=========================================================================================================
/// SHVControlImplementerEditWin32 - Edit box implementation
/**
 */

class SHVControlImplementerEditWin32 : public SHVControlImplementerWin32<SHVControlImplementerEdit>
{
public:


	SHVControlImplementerEditWin32(int subType = SHVControlEdit::SubTypeSingleLine);


	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);

	virtual SHVBool SetFlag(SHVControl* owner, int flag, bool enable);
	virtual bool GetFlag(SHVControl* owner, int flag);

	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);


	// From SHVControlImplementerContainer
	virtual SHVStringBuffer GetText();
	virtual void SetText(const SHVStringC& text);

	virtual int GetLimit();
	virtual void SetLimit(int limit);

	virtual void SetHeight(SHVControlEdit* owner, int lines); // only applicable on multiline boxes

	// From SHVControlImplementerWin32
	virtual int CalculateNewHeight(SHVControl* owner, SHVFont* font);


private:
	///\cond INTERNAL
	int SubType;
	int Lines;
	///\endcond
};

#endif
