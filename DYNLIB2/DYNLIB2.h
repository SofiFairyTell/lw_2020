#pragma once
#if !defined(_DYN_LIB_2_H_)
#define _DYN_LIB_2_H_
	#ifdef DYNLIB2_EXPORTS
	#define DYNLIB2_API _declspec(dllexport)
	#else
	#define DYNLIB2_API _declspec(dllimport)
	#endif // 
#endif
typedef void(*PRINT_SYSMETR_PROC)(LPCWSTR LpDisplayNam, int index);
#ifdef DYNLIB2_PROTOTYPES
#ifdef __cplusplus
extern "C"
{
#endif
	DYNLIB2_API void PrintSYSmetr(LPCWSTR LpDisplayNam, int index);
#ifdef __cplusplus

#endif // __cplusplus

}
#endif // DYNLIB2_PROTOTYPES



