#pragma once
#if !defined(_DYN_LIB_1_H_)
	#define _DYN_LIB_1_H_
	#ifdef DYNLIB1_EXPORTS
		#define DYNLIB1_API __declspec(dllexport)
	#else
		#define DYNLIB1_API __declspec(dllimport)
	#endif // 
#endif // !!defined(_DYN_LIB_1_H)
#ifdef __cplusplus

//extern "C" IObject *__stdcall Info();
//struct IObject
//{
//	DYNLIB1_API void TimeDateInfo(
//		LPCWSTR lplocalname,
//		DWORD dwFlags,
//		const SYSTEMTIME * lpDate,
//		LPCWSTR lpDateFormat,
//		LPCWSTR lpTimeFormat);
//	DYNLIB1_API void TimeDateInfo(
//		LPCWSTR lplocalname,
//		DWORD dwFlags,
//		LPCWSTR lpDateFormat,
//		LPCWSTR lpTimeFormat);
//};
//extern "C"
//{
//#endif
	DYNLIB1_API void PrintSysDir(const long csidl[], unsigned long nCount);
	DYNLIB1_API void PrintOSinfo();
	DYNLIB1_API void TimeDateInfo(LPCWSTR lplocalname,DWORD dwFlags,const SYSTEMTIME * lpDate,LPCWSTR lpDateFormat,LPCWSTR lpTimeFormat);
	DYNLIB1_API void TimeDateInfo(LPCWSTR lplocalname, DWORD dwFlags, LPCWSTR lpDateFormat, LPCWSTR lpTimeFormat);
	//#ifdef __cplusplus
//}

#endif // _cplusplus

