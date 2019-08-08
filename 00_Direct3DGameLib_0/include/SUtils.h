#pragma once

#include <WinSock2.h>
#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <math.h>
#include <process.h>

#include <set>
#include <list>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <bitset>
#include <limits>
#include <functional>
#include <iostream>
#include <fstream>


using namespace std;

typedef basic_string<TCHAR>				T_STR;
typedef basic_string<TCHAR>::iterator	T_ITOR;

typedef vector<basic_string<TCHAR>>	T_STR_VECTOR;
typedef vector<string>				C_STR_VECTOR;

typedef list<basic_string<TCHAR>>	T_STR_LIST;
typedef string						CHAR_STRING;

typedef vector<DWORD>				DWORD_VECTOR;
typedef	vector< DWORD >::iterator	DWORD_VECTOR_ITOR;

typedef list<DWORD>					DWORD_LIST;
typedef list<DWORD>::iterator		DWORD_LIST_ITOR;

typedef list< HANDLE >				HANDLE_LIST;
typedef	list< HANDLE >::iterator	HANDLE_LIST_ITOR;


#ifdef _WIN64
	#pragma comment(lib, "fmod64_vc.lib")
#else
	#pragma comment(lib, "fmod_vc.lib")
#endif // _WIN64



#define SAMPLE_BUFFER_SIZE	16 
#define KEYSTATECOUNT		256

//////////////////////////////////////////////
// 객체 및 배열 할당과 삭제 및 소멸 매크로
//////////////////////////////////////////////
#ifndef SAFE_ZERO
#define SAFE_ZERO(A)				{ A = 0; }
#endif

#ifndef SAFE_NEW
#define SAFE_NEW(A, B)				{ if (A == nullptr ) A = new B; }
#endif 

#ifndef SAFE_DEL
#define SAFE_DEL(A)					{ if (A != nullptr ) delete A; (A)= nullptr; }
#endif 

#ifndef SAFE_NEW_ARRAY
#define SAFE_NEW_ARRAY(A, B, C)		{ if (A == nullptr  && C) A = new B[C]; }
#endif 

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(A)		{ if (A != nullptr ) delete [] A; (A)= nullptr; }
#endif 

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(A)				{ if(A != nullptr ) { (A)->Release(); (A) = nullptr; } }
#endif 

#ifndef SAFE_NEW_CLEAR
#define SAFE_NEW_CLEAR( A, B )			{ if (A == nullptr ) A = new B; if(A) memset( A, 0, sizeof(B) ); };
#endif 

#ifndef SAFE_NEW_ARRAY_CLEAR
#define NEW_ARRAY_CLEAR( A, B, C )	{ if (A != nullptr  && C) A = new B[C]; if(A) memset( A, 0, sizeof(B)*C ); };
#endif

#ifndef HRESULT_FAILED_RETURN
#define HRESULT_FAILED_RETURN(hr)	{ if (FAILED(hr)) return hr; }
#endif

#ifndef HR_SUCCEDED_RETURN
#define HR_SUCCEDED_RETURN(hr)	{ if (SUCCEDED(hr)) return hr; }
#endif

#ifndef HRESULT_FAILED_RETURN_BOOL
#define HRESULT_FAILED_RETURN_BOOL(hr)	{ if (FAILED(hr)) return false; }
#endif

#ifndef HR_SUCCEDED_RETURN_BOOL
#define HR_SUCCEDED_RETURN_BOOL(hr)	{ if (SUCCEDED(hr)) return true; }
#endif

#define _CRT_SECURE_NO_WARNINGS

#define KEY_FREE 0
#define KEY_PUSH 1
#define KEY_HOLD 2
#define KEY_UP 3
#define KEY_ERROR 4