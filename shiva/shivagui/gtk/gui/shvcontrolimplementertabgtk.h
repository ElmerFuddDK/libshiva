#ifndef __SHIVA_GTKGUI_SHVCONTROLIMPLEMENTERGTK_H
#define __SHIVA_GTKGUI_SHVCONTROLIMPLEMENTERGTK_H

#include "../../../include/gui/shvcontroltab.h"
#include "../../../include/gui/shvcontrolcontainer.h"
#include "../../../include/gui/shvformbase.h"
#include "shvcontrolimplementergtkwidget.h"



//-=========================================================================================================
/// SHVControlImplementerTabGtk
/**
 * Implementation of a label implementer for GTK
 */

class SHVControlImplementerTabGtk : public SHVControlImplementerGtkWidget<SHVControlImplementerTab>
{
public:

	SHVControlImplementerTabGtk();

	// From SHVControlImplementer
	virtual int GetSubType(SHVControl* owner);
	virtual SHVBool Create(SHVControl* owner, SHVControlImplementer* parent, int flags);

	// From SHVControlImplementerTab
	virtual SHVControlContainer* AddPage(SHVControlTab* owner, const SHVStringC name, size_t atIndex = SIZE_T_MAX);
	virtual SHVControlContainer* GetPage(SHVControlTab* owner, size_t index);
	virtual void RemovePage(SHVControlTab* owner, size_t index);
	virtual void RemovePageByContainer(SHVControlTab* owner, SHVControlContainer* container);
	virtual void Clear(SHVControlTab* owner);

	virtual size_t GetSelectedPage(SHVControlTab* owner);
	virtual void SelectPage(SHVControlTab* owner, size_t index);

	virtual void SetPageForm(SHVControlTab* owner, size_t index, SHVFormBase* form);
	virtual SHVFormBase* GetPageForm(SHVControlTab* owner, size_t index);

	virtual SHVStringBuffer GetPageName(SHVControlTab* owner, size_t index);

	virtual size_t GetPageCount(SHVControlTab* owner);


private:

	///\cond INTERNAL
	static gboolean on_current_page_changed(GtkNotebook *, gint arg1, gpointer);
	static gboolean on_unrealize(GtkWidget *widget, gpointer data);
	
	struct PageItem {
		SHVControlContainerRef Container;
		SHVFormBaseRef Form;
		SHVString Name;

		inline PageItem(const SHVStringC name) : Name(name) {}
	};

	SHVVector<PageItem> Pages;
	///\endcond
};

#endif
