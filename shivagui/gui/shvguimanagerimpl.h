#ifndef __SHIVA_GUIIMPL_MANAGER_H
#define __SHIVA_GUIIMPL_MANAGER_H

#include "../include/shvguimanager.h"
#include "../../include/utils/shvhashtable.h"


//-=========================================================================================================
/// SHVGUIManagerImpl - Module for registering control types, and instantiating them
/**
 * This class is a SHIVA module that can be used to keeping default control factories, as well as
 * allowing custom control factories to be registered.
 */

class SHVGUIManagerImpl : public SHVGUIManager
{
public:

	// Constructor
	SHVGUIManagerImpl(SHVModuleList& modules);


	// From SHVModule
	virtual SHVBool Register();
	virtual void Unregister();


	// Register controls
	virtual SHVBool RegisterFactory(int controlType, int controlSubType, SHVControlCreatorBase* factory);
	virtual SHVBool RegisterFactory(const SHVString8C& controlName, SHVControlCreatorBase* factory);


	// Control factory
	virtual SHVControl* CreateControl(int controlType, int controlSubType);
	virtual SHVControl* CreateControl(const SHVString8C& controlName);


	virtual SHVModuleList& GetModuleList();
	virtual SHVConfig& GetConfig();


protected:
	///\cond INTERNAL
	typedef struct ControlPair {
		int Type;
		int SubType;
		
		inline ControlPair(int type, int subType) : Type(type), SubType(subType)  {}
		inline operator size_t() const { return Type+(SubType<<4); }
		inline bool operator==(const ControlPair& p) const { return p.Type == Type && p.SubType == SubType; }
	};
	
	SHVHashTable<ControlPair,SHVControlCreatorBaseRef> ControlsByType;
	SHVHashTable<SHVString8,SHVControlCreatorBaseRef,const SHVString8C> ControlsByName;
	///\endcond
};

#endif
