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
			return ErrorCloseStream(_T("������ ã�� ���߽��ϴ�."));
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
	// ��ü���Ͽ��� �ش� ���ڿ��� ã���� ���� �˻��ϰ� ���������͸� ������������ �ű��.
	BOOL	GetDataFromFileLoop(const TCHAR* pString = nullptr, VOID* pData = nullptr, INT DataType = NULL_DATA);
	// ���������� ������� ���忭�� ã�� ������������ ���������͸� �ű��.
	BOOL	GetDataFromFileNext(const TCHAR* pString = nullptr, VOID* pData = nullptr, INT DataType = NULL_DATA);
	// ���ϴ� ���ڿ��� ã�� ã�� ���ڿ� ������ġ�� �ű��
	BOOL	GetDataFromFileString(const TCHAR* pString = nullptr, VOID* pData = nullptr, INT DataType = NULL_DATA);
	// ���������� ������� �ش繮�ڿ��� ã�� �˻��� �����ϸ� ���� �������� ���������͸� �ű��.
	BOOL	GetDataFromFile(const TCHAR* pString = nullptr, VOID* pData = nullptr, INT DataType = NULL_DATA);
	BOOL    GetNodeName(const TCHAR* pString = nullptr, VOID* pData = nullptr, INT DataType = NULL_DATA);
public:
	SParser();
	virtual ~SParser();
};

