#pragma once
#include "SDxBasic.h"
#include "SUtils.h"
#include "SObjStd.h"

#define _CRT_SECURE_NO_WARNINGS

enum SET_DATA_TYPES
{
	// Gameplay semantics
	NULL_DATA = 0,
	BOOL_DATA,
	INT_DATA,
	FLOAT_DATA,
	STRING_DATA,
	VERTEX_DATA,
	VECTOR_DATA,
	INDEX_DATA,
	FACE_DATA,
	MATRIX_DATA,
};


class SParser
{
public:
	FILE*	m_pStream;
	TCHAR	m_pBuffer[MAX_PATH];
	TCHAR	m_pString[MAX_PATH];
public:
	//================================================================================
	// File Open / Close
	//================================================================================
	BOOL	OpenStream(const TCHAR* pFileName)
	{
		_tfopen_s(&m_pStream, pFileName, _T("rb"));
		if (m_pStream == NULL)
		{
			return ErrorCloseStream(_T("파일을 찾지 못했습니다."));
		}
		return TRUE;
	}
	BOOL    CloseStream()
	{
		fclose(m_pStream);
		return TRUE;
	}
	bool	ErrorCloseStream(const TCHAR* pMsg)
	{
		if (!m_pStream) return false;
		fclose(m_pStream);
		if (pMsg != NULL)
		{
			DEBUGMSG(pMsg);
		}
		return true;
	}
public:
	BOOL	ComparisonData(const TCHAR* pDestString, const TCHAR* pSrcString = nullptr);
	
	BOOL	GetData(VOID* pData, INT DataType);
	void	GetNextLine();
	// 전체파일에서 해당 문자열을 찾을때 까지 검색하고 파일포인터를 다음라인으로 옮긴다.
	BOOL	GetDataFromFileLoop(const TCHAR* pString = nullptr, VOID* pData = nullptr, INT DataType = NULL_DATA);
	// 다음라인을 대상으로 문장열을 찾고 다음라인으로 파일포인터를 옮긴다.
	BOOL	GetDataFromFileNext(const TCHAR* pString = nullptr, VOID* pData = nullptr, INT DataType = NULL_DATA);
	// 원하는 문자열을 찾고 찾은 문자열 시작위치로 옮긴다
	BOOL	GetDataFromFileString(const TCHAR* pString = nullptr, VOID* pData = nullptr, INT DataType = NULL_DATA);
	// 다음라인을 대상으로 해당문자열을 찾고 검색을 실폐하면 이전 라인으로 파일포인터를 옮긴다.
	BOOL	GetDataFromFile(const TCHAR* pString = nullptr, VOID* pData = nullptr, INT DataType = NULL_DATA);
	BOOL    GetNodeName(const TCHAR* pString = nullptr, VOID* pData = nullptr, INT DataType = NULL_DATA);
public:
	SParser();
	virtual ~SParser();
};

