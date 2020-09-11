#pragma once
#if !defined(_DYN_LIB_1_H_)
	#define _DYN_LIB_1_H_
	#ifdef DYNLIB1_EXPORTS
		#define DYNLIB1_API _declspec(dllexport)
	#else
		#define DYNLIB1_API _declspec(dllimport)
	#endif // 
#endif // !!defined(_DYN_LIB_1_H)
#ifdef __cplusplus
extern "C"
{
#endif
	DYNLIB1_API void PrintSysDir(const long csidl[], unsigned long nCount);
	DYNLIB1_API void PrintOSinfo();
#ifdef __cplusplus

}
#endif // _cplusplus

