#pragma once
#if !defined(_DYN_LIB_2_H_)
#define _DYN_LIB_2_H_
	#ifdef DYNLIB2_EXPORTS
	#define DYNLIB2_API _declspec(dllexport)
	#else
	#define DYNLIB2_API _declspec(dllimport)
	#endif // 
#endif


