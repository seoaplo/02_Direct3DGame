#pragma once
#include "SDxBasic.h"
#include "SUtils.h"

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
	TCHAR	m_pBuffer[256];
	TCHAR	m_pString[256];
public:
	BOOL	ComparisonData(const TCHAR* pDestString, const TCHAR* pSrcString = nullptr);
	bool	ErrorCloseStream(const TCHAR* pMsg);
	BOOL	OpenStream(const TCHAR* pFileName);
	BOOL    CloseStream() { fclose(m_pStream); return TRUE; };
	BOOL	GetData(VOID* pData, INT DataType);
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

