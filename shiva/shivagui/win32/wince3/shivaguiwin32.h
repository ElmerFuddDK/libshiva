
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SHIVAGUIWIN32_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SHIVAGUIWIN32_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef SHIVAGUIWIN32_EXPORTS
#define SHIVAGUIWIN32_API __declspec(dllexport)
#else
#define SHIVAGUIWIN32_API __declspec(dllimport)
#endif

// This class is exported from the shivaguiwin32.dll
class SHIVAGUIWIN32_API CShivaguiwin32 {
public:
	CShivaguiwin32(void);
	// TODO: add your methods here.
};

extern SHIVAGUIWIN32_API int nShivaguiwin32;

SHIVAGUIWIN32_API int fnShivaguiwin32(void);

