#ifndef _SHIVA_MODULEFACTORY_JSONSTREAM_H
#define _SHIVA_MODULEFACTORY_JSONSTREAM_H

#include "shiva/include/framework/shvmodulefactory.h"


//-=========================================================================================================
/// SHVModuleFactoryJsonStream - factory for parsing/generation Json (Java simple object notation)
/**
 */

class SHVModuleFactoryJsonStream : public SHVModuleFactory
{
public:


  // Constructor
  SHVModuleFactoryJsonStream(SHVModuleList& modules);


  virtual SHVBool CheckVersion(int verMajor,int verMinor,int verRelease);
  virtual SHVInt GetMajor();
  virtual SHVInt GetMinor();
  virtual SHVInt GetRelease();
  virtual SHVInt GetBuild();
  virtual SHVStringBuffer GetName();


  // Get description
  virtual SHVStringBuffer GetDescription();


  // Module instantiation
  virtual SHVBool ResolveModules(const SHVString8C symbol);


  // from SHVEventTarget
protected:
  virtual void OnEvent(SHVEvent* event);

private:
  SHVModuleList& Modules;
};

#endif
