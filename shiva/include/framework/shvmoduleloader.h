#ifndef __SHIVA_FRAMEWORK_MODULELOADER_H
#define __SHIVA_FRAMEWORK_MODULELOADER_H

#include "shvmodule.h"
#include "shvmodulefactory.h"

// forward declares
class SHVModuleLibIterator;
class SHVModuleLibrary;


//-=========================================================================================================
/// SHVModuleLoader class - interface for module loader
/**
 */

class SHVAPI SHVModuleLoader : public SHVModule
{
public:


	// Module library load functions
	virtual void AddModuleLibs(const SHVStringC modulePath) = 0;
	virtual SHVBool AddModuleLib(const SHVStringC fileName) = 0;
	virtual SHVBool AddModuleFactory(SHVModuleFactory* moduleFactory) = 0;
	virtual SHVModuleLibIterator* GetIterator() = 0;
	virtual SHVModuleLibrary* GetModuleLib(const SHVStringC name) = 0;


	// Module symbol functions
	virtual void AddSymbol(const SHVString8C symbolName) = 0;
	virtual SHVBool ContainsSymbol(const SHVString8C symbolName) = 0;


	// Load modules
	virtual SHVBool LoadModules() = 0;


protected:
	// constructor
	inline SHVModuleLoader(SHVModuleList &modules) : SHVModule(modules,"ModuleLoader") {}
};


//-=========================================================================================================
/// SHVModuleLibrary class - interface for a module library for the module loader
/**
 */

class SHVAPI SHVModuleLibrary
{
public:


	// Properties
	virtual const SHVStringC GetLibPath() const = 0;
	virtual const SHVStringC GetLibName() const = 0;
	virtual const SHVStringC GetName() const = 0;
	virtual const SHVStringC GetDescription() const = 0;


	virtual SHVInt GetVersionMajor() const = 0;
	virtual SHVInt GetVersionMinor() const = 0;
	virtual SHVInt GetVersionRelease() const = 0;
	virtual SHVInt GetBuild() const = 0;


	// Query functions
	virtual SHVBool InUse() const = 0;
	virtual SHVModuleFactory* GetFactory() = 0;


protected:
friend class SHVModuleLoader;

	virtual ~SHVModuleLibrary() {}
};


//-=========================================================================================================
/// SHVModuleLibIterator class - iterator for module libraries
/**
 */

class SHVAPI SHVModuleLibIterator
{
public:
	virtual operator SHVBool() = 0;
	virtual SHVBool MoveNext() = 0;
	virtual SHVModuleLibrary* Get() = 0;

	virtual ~SHVModuleLibIterator() {}
};


// ============================================= implementation ============================================= //
/// \class SHVModuleLoader shvmoduleloader.h "shiva/include/framework/shvmoduleloader.h"
/// \class SHVModuleLibrary shvmoduleloader.h "shiva/include/framework/shvmoduleloader.h"
/// \class SHVModuleLibIterator shvmoduleloader.h "shiva/include/framework/shvmoduleloader.h"
#endif
