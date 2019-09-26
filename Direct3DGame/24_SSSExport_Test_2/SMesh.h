#pragma once
#include "SModel.h"

class SMesh : public SModel
{
protected:
	T_STR	m_FilePath;
	T_STR	m_MeshDataName;
public:
	virtual bool Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* pMeshDataName, const TCHAR* pLoadShaderFile );
	virtual bool Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,const TCHAR* pMeshDataName, const TCHAR* pLoadTextureString, const TCHAR* pLoadShaderFile);
public:
	bool LoadExportFile();
	bool CreateVertexFromFile(FILE* pStream, int iMaxVertexData);
	bool CreateIndexFromFile(FILE* pStream, int iMaxIndexData);
public:
	SMesh();
	virtual ~SMesh();
};

