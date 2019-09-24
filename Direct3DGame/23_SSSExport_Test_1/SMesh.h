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
	virtual bool PostRender(ID3D11DeviceContext* pContext);
public:
	bool LoadExportFile();
	bool CreateShaderFile(FILE* pStream, int iVertexData);
public:
	SMesh();
	virtual ~SMesh();
};

