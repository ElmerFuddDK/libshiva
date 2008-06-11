#ifndef __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERTAB_H
#define __SHIVA_WIN32GUI_SHVCONTROLIMPLEMENTERTAB_H


#include "../../../include/gui/shvcontroltab.h"
#include "../../../include/gui/shvcontrolcontainer.h"
#include "../../../include/gui/shvformbase.h"
#include "shvcontrolimplementerwin32.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif


//-=========================================================================================================
/// SHVControlImplementerTabWin32 - tab implementation
/**
 */

class SHVControlImplementerTabWin32 : public SHVControlImplementerWin32<SHVControlImplementerTab>
{
public:


	SHVControlImplementerTabWin32();


	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);

	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);


	// From SHVControlImplementerTab
	virtual SHVControlContainer* AddPage(SHVControlTab* owner, const SHVStringC name, size_t atIndex = SIZE_T_MAX);
	virtual SHVControlContainer* GetPage(SHVControlTab* owner, size_t index);
	virtual void RemovePage(SHVControlTab* owner, size_t index);
	virtual void Clear(SHVControlTab* owner);

	virtual size_t GetSelectedPage(SHVControlTab* owner);
	virtual void SelectPage(SHVControlTab* owner, size_t index);

	virtual void SetPageForm(SHVControlTab* owner, size_t index, SHVFormBase* form);
	virtual SHVFormBase* GetPageForm(SHVControlTab* owner, size_t index);

	virtual SHVStringBuffer GetPageName(SHVControlTab* owner, size_t index);

	virtual size_t GetPageCount(SHVControlTab* owner);


private:
	///\cond INTERNAL
	WNDPROC OrigProc;

	struct PageItem {
		SHVControlContainerRef Container;
		SHVFormBaseRef Form;
		SHVString Name;

		inline PageItem(const SHVStringC name) : Name(name) {}
	};

	SHVVector<PageItem> Pages;
	SHVControlContainer* CurrentContainer;

	void SetContainerSize();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	///\endcond
};

#endif
