#pragma once
#if !defined(_DYN_LIB_3_H_)
#define _DYN_LIB_3_H_

#ifdef DYNLIB3_EXPORTS
#define DYNLIB3_API __declspec(dllexport)
#else
#define DYNLIB3_API __declspec(dllimport)
#endif // 

#ifndef SPI_GETMOUSEWHEELROUTNG
#define SPI_GETMOUSEWHEELROUTING 0x201C
#endif // !SPI_GETMOUSEWHEELROUTNG

#ifdef __cplusplus
extern "C"
{
#endif

	DYNLIB3_API BOOL PrintSYSparamInfo(LPCWSTR lpDispalyNam, UINT uiAction);
#ifdef  __cplusplus
}
#endif // __cplusplus
#endif // _DYN_LIB_3_H_

