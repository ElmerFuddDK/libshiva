#ifndef __SHIVA_FRAMEWORK_MODULELOADERIMPL_H
#define __SHIVA_FRAMEWORK_MODULELOADERIMPL_H

#include "../framework/shvmoduleloader.h"

#include "../utils/shvdll.h"
#include "../utils/shvhashtable.h"

// forward declare
class SHVModuleLibraryImpl;
typedef SHVPtrContainer<SHVModuleLibraryImpl> SHVModuleLibraryImplPtr;



//-=========================================================================================================
/// SHVModuleLoaderImpl class - implementation of module loader
/**
 * Contains the implementation of the module loader
 */

class SHVAPI SHVModuleLoaderImpl : public SHVModuleLoader
{
public:


	// constructor
	SHVModuleLoaderImpl(SHVModuleList &modules);
	virtual ~SHVModuleLoaderImpl();


	// Module library load functions
	virtual void AddModuleLibs(const SHVStringC modulePath);
	virtual SHVBool AddModuleLib(const SHVStringC fileName);
	virtual SHVBool AddModuleFactory(SHVModuleFactory* moduleFactory);
	virtual SHVModuleLibIterator* GetIterator();
	virtual SHVModuleLibrary* GetModuleLib(const SHVStringC name);


	// Module symbol functions
	virtual void AddSymbol(const SHVString8C symbolName);
	virtual SHVBool ContainsSymbol(const SHVString8C symbolName);


	// Load modules
	virtual SHVBool LoadModules();


private:
friend class SHVModuleLibIteratorImpl;

	///\cond INTERNAL
	SHVBool AddModuleLibInternal(SHVModuleLibraryImpl* moduleLib);
	
	SHVList<SHVString8,const SHVString8C> ModuleSymbols;

	SHVList<SHVModuleLibraryImplPtr,SHVModuleLibraryImpl*> ModuleLibs;
	SHVList<SHVModuleLibraryImpl*> ModulesInUse;

	SHVHashTable<SHVModuleLibraryImpl*,int> UnusedModules;
	SHVHashTableString<SHVModuleLibraryImpl*> ModuleNameMap;
	///\endcond
};



//-=========================================================================================================
/// SHVModuleLibraryImpl class - implementation of module library for module loader
/**
 */

class SHVAPI SHVModuleLibraryImpl : public SHVModuleLibrary
{
public:


	SHVModuleLibraryImpl();
	~SHVModuleLibraryImpl();


	virtual const SHVStringC GetLibPath() const;
	virtual const SHVStringC GetLibName() const;
	virtual const SHVStringC GetName() const;
	virtual const SHVStringC GetDescription() const;


	virtual SHVInt GetVersionMajor() const;
	virtual SHVInt GetVersionMinor() const;
	virtual SHVInt GetVersionRelease() const;
	virtual SHVInt GetBuild() const;


	virtual SHVBool InUse() const;
	virtual SHVModuleFactory* GetFactory();

private:
friend class SHVModuleLoaderImpl;

	///\cond INTERNAL
	SHVDll Library;
	SHVModuleFactoryPtr ModuleFactory;
	SHVBool Used;

	SHVString LibraryPath;
	SHVString LibraryName;
	SHVString Name;
	SHVString Description;

	SHVInt VersionMajor, VersionMinor, VersionRelease;
	SHVInt Build;
	///\endcond
};



//-=========================================================================================================
/// SHVModuleLibIteratorImpl class - iterator for module libraries
/**
 */

class SHVAPI SHVModuleLibIteratorImpl : public SHVModuleLibIterator
{
public:


	SHVModuleLibIteratorImpl(SHVModuleLoaderImpl& moduleLoader);


	virtual operator SHVBool();
	virtual SHVBool MoveNext();
	virtual SHVModuleLibrary* Get();


private:
	///\cond INTERNAL
	SHVListIterator<SHVModuleLibraryImpl*> Iterator;
	///\endcond
};

#endif
