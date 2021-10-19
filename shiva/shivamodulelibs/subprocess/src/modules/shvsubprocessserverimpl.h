#ifndef __SHIVA_SUBPROCESSIMPL_SUBPROCESSSERVER_H
#define __SHIVA_SUBPROCESSIMPL_SUBPROCESSSERVER_H

#include "../../../../include/modules/subprocess/shvsubprocessserver.h"
#include "../../../../include/threadutils/shvmutexbase.h"
#include "../../../../include/threadutils/shvthread.h"
#include "../../../../include/utils/shvlist.h"
#include "../../../../include/utils/shvdll.h"
class SHVSubProcessImpl;


//-=========================================================================================================
/// SHVSubProcessImpl class
//-=========================================================================================================
/**
 * Implementation of SHVSubProcess
 */

class SHVSubProcessServerImpl : public SHVSubProcessServer
{
public:


	// constructor
	SHVSubProcessServerImpl(SHVModuleList& modules);
	~SHVSubProcessServerImpl();


	// module functions
	virtual SHVBool Register();
	virtual void PostRegister();
	virtual void Unregister();


	// Create a sub process
	virtual SHVSubProcess* New();


private:
friend class SHVSubProcessImpl;
	SHVMutexBase StatusLock;
#ifdef __SHIVA_WIN32

	struct ActiveProcess {
		HANDLE hProcess;
		SHVSubProcessImpl* Owner;
		HANDLE hListenProcess;
		bool Closing;
		inline ActiveProcess(HANDLE hProc, SHVSubProcessImpl* obj) : hProcess(hProc), Owner(obj) { hListenProcess = INVALID_HANDLE_VALUE; Closing = false; }
	};

	SHVMutexBase ActiveProcessLock;
	SHVList<ActiveProcess> ActiveProcesses;
	SHVThread<SHVSubProcessServerImpl> ActiveProcessMonitorThread;

	SHVDllBase Kernel32;
	BOOL(WINAPI *CancelIoEx)(HANDLE hFile, LPOVERLAPPED lpOverlapped);

	HANDLE hThreadSignaller;
	bool Running;

	void ActiveProcessMonitor();
	void AddActiveProcess(HANDLE hProcess, SHVSubProcessImpl* owner);
	void RemoveActiveProcess(HANDLE hProcess, SHVSubProcessImpl* owner);
#endif
};

#endif
