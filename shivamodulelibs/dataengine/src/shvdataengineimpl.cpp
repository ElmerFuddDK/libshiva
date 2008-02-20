#include "stdafx.h"

#include "../../../../include/platformspc.h"
#include "../../include/dataengineimpl/shvdatafactory_impl.h"
#include "../../include/dataengineimpl/shvdataengine_impl.h"


SHVDataEngine_impl::SHVDataEngine_impl(SHVModuleList& modules): SHVModule(modules, _T("SHVDataEngine"), NULL)
{
}

SHVBool SHVDataEngine_impl::Register()
{
SHVBool retVal = SQLiteDll.Load(SQLiteDll.CreateLibFileName("shvsqlite"));
	if (!retVal)
	{
		Modules.AddStartupError(_T("Could not load shvsqlite.dll"));
	}
	return SHVModule::Register() && retVal;
}

void SHVDataEngine_impl::Unregister()
{
}

SHVDataFactory* SHVDataEngine_impl::CreateFactory(const SHVStringC& dbName)
{
	return new SHVDataFactory_impl((SHVSQLiteWrapper*) dll.CreateObjectInt(NULL, SHVDll::ClassTypeUser));
}


