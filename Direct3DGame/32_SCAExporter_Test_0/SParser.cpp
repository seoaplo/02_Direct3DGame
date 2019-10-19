#include "SParser.h"

BOOL SParser::ComparisonData(const TCHAR* pDestString, const TCHAR* pSrcString)
{
	if (pSrcString == NULL)
	{
		pSrcString = m_pString;
	}
	if (!_tcsicmp(pSrcString, pDestString))
	{
		return TRUE;
	}
	return FALSE;
}
BOOL SParser::GetData(VOID* pData, INT DataType)
{
	if (pData != NULL)
	{
		switch (DataType)
		{
		case INT_DATA:
			_stscanf_s(m_pBuffer, _T("%s%d"), m_pString, MAX_PATH, pData);
			break;
		case FLOAT_DATA:
			_stscanf_s(m_pBuffer, _T("%s%f"), m_pString, MAX_PATH, pData);
			break;
		case STRING_DATA:
		{
			TCHAR szTexPath[MAX_PATH] = TEXT("");
			_stscanf_s(m_pBuffer, _T("%s %s"), m_pString, MAX_PATH, szTexPath);
			if (szTexPath[0] == '"')
			{
				// 문자열의 시작과 끝에 있는 (")문자를 삭제한다.
				// 또한 문자열중간에 공백까지도 반환문자열에 포함한다.
				_tcscpy_s(szTexPath, MAX_PATH, _tcsstr(m_pBuffer, _T("\"")));
				size_t filelen = _tcslen(szTexPath);
				szTexPath[filelen - 2] = 0;
				_tcscpy_s((TCHAR*)pData, MAX_PATH, &szTexPath[1]);
			}
			else
			{
				_tcscpy_s((TCHAR*)pData, MAX_PATH, &szTexPath[0]);
			}
		}
		break;
		case VERTEX_DATA:
		{
			_stscanf_s(m_pBuffer, _T("%f %f %f"),
				&((PNCT_VERTEX*)pData)->p.x,
				&((PNCT_VERTEX*)pData)->p.y,
				&((PNCT_VERTEX*)pData)->p.z);

			_stscanf_s(m_pBuffer, _T(" %f %f %f"),
				&((PNCT_VERTEX*)pData)->n.x,
				&((PNCT_VERTEX*)pData)->n.y,
				&((PNCT_VERTEX*)pData)->n.z);

			_stscanf_s(m_pBuffer, _T(" %f %f %f"),
				&((PNCT_VERTEX*)pData)->c.x,
				&((PNCT_VERTEX*)pData)->c.y,
				&((PNCT_VERTEX*)pData)->c.z,
				&((PNCT_VERTEX*)pData)->c.w);

			_stscanf_s(m_pBuffer, _T("%f %f %f"),
				&((PNCT_VERTEX*)pData)->t.x,
				&((PNCT_VERTEX*)pData)->t.y);
		}
		break;
		case FACE_DATA:
		case MATRIX_DATA:
			_stscanf_s(m_pBuffer, _T("%f %f %f %f"),
				&((D3DMATRIX*)pData)->_11,
				&((D3DMATRIX*)pData)->_12,
				&((D3DMATRIX*)pData)->_13,
				&((D3DMATRIX*)pData)->_14);

			_stscanf_s(m_pBuffer, _T("%f %f %f %f"),
				&((D3DMATRIX*)pData)->_21,
				&((D3DMATRIX*)pData)->_22,
				&((D3DMATRIX*)pData)->_23,
				&((D3DMATRIX*)pData)->_24);

			_stscanf_s(m_pBuffer, _T("%f %f %f %f"),
				&((D3DMATRIX*)pData)->_31,
				&((D3DMATRIX*)pData)->_32,
				&((D3DMATRIX*)pData)->_33,
				&((D3DMATRIX*)pData)->_34);

			_stscanf_s(m_pBuffer, _T("%f %f %f %f"),
				&((D3DMATRIX*)pData)->_41,
				&((D3DMATRIX*)pData)->_42,
				&((D3DMATRIX*)pData)->_43,
				&((D3DMATRIX*)pData)->_44);
		case NULL_DATA:
			break;
		}
	}
	return TRUE;
}
void SParser::GetNextLine()
{
	_fgetts(m_pBuffer, 256, m_pStream);
}
BOOL SParser::GetDataFromFileNext(const TCHAR* pString, VOID* pData, INT DataType)
{

	if (!feof(m_pStream))
	{
		_fgetts(m_pBuffer, 256, m_pStream);
		_stscanf_s(m_pBuffer, _T("%s"), m_pString, MAX_PATH);
		if (!_tcsicmp(m_pString, pString))
		{
			return GetData(pData, DataType);
		}
	}

	return FALSE;
}

BOOL SParser::GetDataFromFile(const TCHAR* pString, VOID* pData, INT DataType)
{

	fpos_t  FilePos;
	if (fgetpos(m_pStream, &FilePos))
	{
		ErrorCloseStream(_T("작업도중 파일포인터를 잃어 버렸습니다."));
	}
	if (!feof(m_pStream))
	{
		_fgetts(m_pBuffer, 256, m_pStream);
		_stscanf_s(m_pBuffer, _T("%s"), m_pString, MAX_PATH);
		if (!_tcsicmp(m_pString, pString))
		{
			return GetData(pData, DataType);
		}
		else
		{
			if (fsetpos(m_pStream, &FilePos))
			{
				ErrorCloseStream(_T("작업도중 파일포인터를 잃어 버렸습니다."));
			}
		}
	}

	return FALSE;
}
// 원하는 문자열을 찾고 찾은 문자열 시작위치로 옮긴다
BOOL SParser::GetDataFromFileString(const TCHAR* pString, VOID* pData, INT DataType)
{
	fpos_t  FilePos;

	while (!feof(m_pStream))
	{
		if (fgetpos(m_pStream, &FilePos))
		{
			ErrorCloseStream(_T("작업도중 파일포인터를 잃어 버렸습니다."));
		}
		_fgetts(m_pBuffer, 256, m_pStream);
		_stscanf_s(m_pBuffer, _T("%s"), m_pString, MAX_PATH);
		if (!_tcsicmp(m_pString, pString))
		{
			if (GetData(pData, DataType))
			{
				if (fsetpos(m_pStream, &FilePos))
				{
					ErrorCloseStream(_T("작업도중 파일포인터를 잃어 버렸습니다."));
				}
			}
			return TRUE;
		}
	}

	return FALSE;
}
BOOL SParser::GetNodeName(const TCHAR* pString, VOID* pData, INT DataType)
{

	fpos_t  FilePos;
	if (fgetpos(m_pStream, &FilePos))
	{
		ErrorCloseStream(_T("작업도중 파일포인터를 잃어 버렸습니다."));
	}
	if (!feof(m_pStream))
	{
		_fgetts(m_pBuffer, 256, m_pStream);
		_stscanf_s(m_pBuffer, _T("%s"), m_pString, MAX_PATH);
		if (!_tcsicmp(m_pString, pString))
		{
			if (pData != NULL)
			{
				switch (DataType)
				{
				case STRING_DATA:
				{
					TCHAR *pStr = NULL;
					TCHAR String1[20];
					TCHAR String2[20];
					TCHAR ResultString[40];
					memset(String1, 0, sizeof(TCHAR) * 20);
					memset(String2, 0, sizeof(TCHAR) * 20);
					memset(ResultString, 0, sizeof(TCHAR) * 40);
					bool check = false;
					int j = 0;
					if (_tcsstr(m_pBuffer, _T("Bip01 R UpperArm")))
					{
						int k;
						k = 0;
					}
					for (int i = 0; i < 256; i++)
					{
						if (check)
						{
							if (m_pBuffer[i] == '"')
							{
								check = false;
								String2[j++] = 0;
								break;
							}
							String2[j++] = m_pBuffer[i];
						}
						else
						{
							if (m_pBuffer[i] == '"')
								check = true;
						}

					}
					//String2[filelen-1] = 0;					
					_tcscpy_s((TCHAR*)pData, MAX_PATH, &String2[0]);
				}
				break;
				}
			}
			return TRUE;
		}
		else
		{
			if (fsetpos(m_pStream, &FilePos))
			{
				ErrorCloseStream(_T("작업도중 파일포인터를 잃어 버렸습니다."));
			}
		}
	}

	return FALSE;
}

BOOL SParser::GetDataFromFileLoop(const TCHAR* pString, VOID* pData, INT DataType)
{
	while (!feof(m_pStream))
	{
		_fgetts(m_pBuffer, 256, m_pStream);
		_stscanf_s(m_pBuffer, _T("%s"), m_pString, MAX_PATH);
		if (!_tcsicmp(m_pString, pString))
		{
			return GetData(pData, DataType);
		}
	}
	return FALSE;//ErrorCloseStream( "전체파일에서 검색할수 없습니다.");	
}

SParser::SParser()
{
}


SParser::~SParser()
{
}
