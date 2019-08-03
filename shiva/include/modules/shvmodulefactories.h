#ifndef __SHIVA_INCLUDE_MODULES_MODULEFACTORIES_H
#define __SHIVA_INCLUDE_MODULES_MODULEFACTORIES_H

extern "C"
{

extern SHVModuleFactory* SHVModuleFactory_DataEngineNew(SHVModuleList* list);
extern SHVModuleFactory* SHVModuleFactory_JsonStreamNew(SHVModuleList* list);
extern SHVModuleFactory* SHVModuleFactory_LuaEngineNew(SHVModuleList* list);
extern SHVModuleFactory* SHVModuleFactory_SocketServerNew(SHVModuleList* list);
extern SHVModuleFactory* SHVModuleFactory_SubProcessNew(SHVModuleList* list);
extern SHVModuleFactory* SHVModuleFactory_XmlStreamNew(SHVModuleList* list);

}

#endif
