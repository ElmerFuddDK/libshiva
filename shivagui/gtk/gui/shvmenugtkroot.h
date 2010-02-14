#ifndef __SHIVA_GTKGUI_SHVMENUGTKROOT_H
#define __SHIVA_GTKGUI_SHVMENUGTKROOT_H

#include "shvmenugtksub.h"
#include "../../../include/utils/shvvectorref.h"
#include "../../../include/utils/shvhashtable.h"

#ifndef __SHIVA_GTK
# error This code only works for gtk
#endif

#include <gtk/gtk.h>
class SHVGUIManagerGtk;


//-=========================================================================================================
/// SHVMenuGtkRoot
/**
 * Interface for a root level menu that holds the information and functions
 * nessecary for the sub menus to be able to emit the event.
 */

class SHVMenuGtkRoot : public SHVMenu
{
public:


	virtual void EmitEvent(SHVInt id) = 0;
	inline void RegisterIDMap(void* key, SHVInt id);
	inline void EmitEventByMap(void* key);
	

protected:
	///\cond INTERNAL
	SHVVectorRef<SHVMenuGtkSub> SubMenus;
	SHVHashTable<void*,SHVInt> EventIDMap;

	// constructor
	inline SHVMenuGtkRoot() {}
	///\endcond
};
typedef SHVRefObjectContainer<SHVMenuGtkRoot> SHVMenuGtkRootRef;



// ============================================= implementation ============================================= //


/*************************************
 * RegisterIDMap
 *************************************/
void SHVMenuGtkRoot::RegisterIDMap(void * key, SHVInt id)
{
	EventIDMap[key] = id;
}

/*************************************
 * EmitEventByMap
 *************************************/
void SHVMenuGtkRoot::EmitEventByMap(void * key)
{
	SHVASSERT(EventIDMap.Find(key));
	EmitEvent(EventIDMap[key]);
}

#endif
