#include "SkinFileLoader.h"

static TCHAR SkinExporter[] = L"SkinExporter100";
static TCHAR Header[] = L"#HEADERINFO";
static TCHAR Material[] = L"#MATERIAL_INFO";
static TCHAR Object[] = L"#OBJECT_INFO";
static TCHAR SubMesh[] = L"SubMesh";
static TCHAR InverseMat[] = L"#INVERSEINFO";

bool SkinFileLoader::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	CreateComputeShader(L"CSBoneVertex.hlsl", "CS", pDevice, &m_pCS);

	return true;
}
bool SkinFileLoader::Load(T_STR FilePath)
{
	if (m_pDevice == nullptr) return -1;
	if (m_pContext == nullptr) return -1;
	SScene	SceneData;
	bool bCheck = false;

	static TCHAR String[MAX_PATH];
	ZeroMemory(String, _countof(String));
	ZeroMemory(&SceneData, sizeof(SceneData));

	m_Paser.OpenStream(FilePath.c_str());

	bCheck = m_Paser.GetDataFromFileNext(SkinExporter);
	if (bCheck == false) return -1;

	bCheck = m_Paser.GetDataFromFile(Header);
	if (bCheck == false) return -1;

	_stscanf_s(m_Paser.m_pBuffer, _T("%s%d%d%d%d%d%d"), String, MAX_PATH,
		&SceneData.iFirstFrame,
		&SceneData.iLastFrame,
		&SceneData.iFrameSpeed,
		&SceneData.iTickPerFrame,
		&SceneData.iNumObjects,
		&SceneData.iNumMaterials);
	ZeroMemory(String, _countof(String));


	bCheck = m_Paser.GetDataFromFileNext(Material);
	if (bCheck == false) return -1;
	// Material

	int iSubMaterialSize = 0;
	for (int iMaterial = 0; iMaterial < SceneData.iNumMaterials; iMaterial++)
	{
		SMaterial* pMaterial = I_MaterialManager.Create();
		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%s%d"), String, MAX_PATH, &iSubMaterialSize);
		pMaterial->m_MaterialName = String;
		ZeroMemory(String, _countof(String));

		// SubMaterial
		for (int iSubMaterial = 0; iSubMaterial < iSubMaterialSize; iSubMaterial++)
		{
			SSubMaterial* pSubMaterial = pMaterial->Create();
			int iTextureSize = 0;
			m_Paser.GetNextLine();
			_stscanf_s(m_Paser.m_pBuffer, _T("%s%d%d"),
				String, MAX_PATH,
				&pSubMaterial->iIndex,
				&iTextureSize);
			pSubMaterial->m_Name = String;
			ZeroMemory(String, _countof(String));

			if (iTextureSize <= 0) continue;

			STextureList& pTextureList = pSubMaterial->m_TextureList;
			// Texture Map
			for (int iTextureMap = 0; iTextureMap < iTextureSize; iTextureMap++)
			{
				int iTextureType = -1;
				m_Paser.GetNextLine();
				_stscanf_s(m_Paser.m_pBuffer, _T("%d%s"),
					&iTextureType,
					String, MAX_PATH);

				T_STR FilePath = L"../../data/Character/";
				FilePath += String;

				int iTextureNumber;
				iTextureNumber = I_TextureManager.Load(m_pDevice, FilePath);
				pTextureList.ConvertToMax(I_TextureManager.GetPtr(iTextureNumber), iTextureType);
				ZeroMemory(String, _countof(String));
			}
		}
	}

	int iInverseMatMaxNum = 0;
	bCheck = m_Paser.GetDataFromFileNext(InverseMat, &iInverseMatMaxNum, INT_DATA);
	if (!bCheck) return -1;
	for (int iCount = 0; iCount < iInverseMatMaxNum; iCount++)
	{
		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&m_InvMatrix[iCount]._11,
			&m_InvMatrix[iCount]._12,
			&m_InvMatrix[iCount]._13,
			&m_InvMatrix[iCount]._14);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&m_InvMatrix[iCount]._21,
			&m_InvMatrix[iCount]._22,
			&m_InvMatrix[iCount]._23,
			&m_InvMatrix[iCount]._24);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&m_InvMatrix[iCount]._31,
			&m_InvMatrix[iCount]._32,
			&m_InvMatrix[iCount]._33,
			&m_InvMatrix[iCount]._34);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&m_InvMatrix[iCount]._41,
			&m_InvMatrix[iCount]._42,
			&m_InvMatrix[iCount]._43,
			&m_InvMatrix[iCount]._44);

		D3DXMatrixTranspose(&m_InvMatrix[iCount], &m_InvMatrix[iCount]);
	}

	CreateStructuredBuffer(m_pDevice, sizeof(D3DXMATRIX),
		iInverseMatMaxNum,
		m_InvMatrix,
		&m_pInvMatBuffer);
	CreateBufferSRV(m_pDevice, m_pInvMatBuffer, &m_pInvMatBufferSRV);

	bCheck = m_Paser.GetDataFromFileNext(Object);
	if (!bCheck) return -1;
	for (int iMesh = 0; iMesh < SceneData.iNumObjects; iMesh++)
	{
		m_Paser.GetNextLine();
		TCHAR ParentName[MAX_PATH];
		ZeroMemory(ParentName, _countof(ParentName));
		int iSubMeshSize = 0;

		SSkinObject* pSkinObject = I_SkinObjectManager.Create();
		_stscanf_s(m_Paser.m_pBuffer, _T("\n%s %s %d %d"),
			String, MAX_PATH,
			ParentName, MAX_PATH,
			&pSkinObject->m_iMaterialID,
			&iSubMeshSize);
		pSkinObject->m_ObjectName = String;
		pSkinObject->m_iMaterialID = I_MaterialManager.GetSize() - SceneData.iNumMaterials + pSkinObject->m_iMaterialID;
		pSkinObject->m_Scene = SceneData;
		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&pSkinObject->m_matObject._11,
			&pSkinObject->m_matObject._12,
			&pSkinObject->m_matObject._13,
			&pSkinObject->m_matObject._14);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&pSkinObject->m_matObject._21,
			&pSkinObject->m_matObject._22,
			&pSkinObject->m_matObject._23,
			&pSkinObject->m_matObject._24);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&pSkinObject->m_matObject._31,
			&pSkinObject->m_matObject._32,
			&pSkinObject->m_matObject._33,
			&pSkinObject->m_matObject._34);

		m_Paser.GetNextLine();
		_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f"),
			&pSkinObject->m_matObject._41,
			&pSkinObject->m_matObject._42,
			&pSkinObject->m_matObject._43,
			&pSkinObject->m_matObject._44);

		for (int iSubMesh = 0; iSubMesh < iSubMeshSize; iSubMesh++)
		{
			int iVertexSize;
			int iIndexSize;
			int iSumMaterialNum;
			SSkinMesh& pSkinMesh = *pSkinObject->CreateMesh();

			bCheck = m_Paser.GetDataFromFileNext(SubMesh);
			if (!bCheck) continue;

			_stscanf_s(m_Paser.m_pBuffer, _T("%s %d %d %d"),
				String, MAX_PATH,
				&iSumMaterialNum,
				&iVertexSize,
				&iIndexSize);
			ZeroMemory(String, _countof(String));

			pSkinMesh._VertexList.resize(iVertexSize);
			pSkinMesh.m_IW_VertexList.resize(iVertexSize);
			pSkinMesh._IndexList.resize(iIndexSize);
			pSkinMesh.iSubMtrlIndex = iSumMaterialNum;
			for (int iVertex = 0; iVertex < iVertexSize; iVertex++)
			{
				m_Paser.GetNextLine();
				_stscanf_s(m_Paser.m_pBuffer, _T("%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f"),
					&pSkinMesh._VertexList[iVertex].p.x,
					&pSkinMesh._VertexList[iVertex].p.y,
					&pSkinMesh._VertexList[iVertex].p.z,
					&pSkinMesh._VertexList[iVertex].n.x,
					&pSkinMesh._VertexList[iVertex].n.y,
					&pSkinMesh._VertexList[iVertex].n.z,
					&pSkinMesh._VertexList[iVertex].c.x,
					&pSkinMesh._VertexList[iVertex].c.y,
					&pSkinMesh._VertexList[iVertex].c.z,
					&pSkinMesh._VertexList[iVertex].c.w,
					&pSkinMesh._VertexList[iVertex].t.x,
					&pSkinMesh._VertexList[iVertex].t.y,

					&pSkinMesh.m_IW_VertexList[iVertex].I1.x,
					&pSkinMesh.m_IW_VertexList[iVertex].I1.y,
					&pSkinMesh.m_IW_VertexList[iVertex].I1.z,
					&pSkinMesh.m_IW_VertexList[iVertex].I1.w,
					&pSkinMesh.m_IW_VertexList[iVertex].I2.x,
					&pSkinMesh.m_IW_VertexList[iVertex].I2.y,
					&pSkinMesh.m_IW_VertexList[iVertex].I2.z,
					&pSkinMesh.m_IW_VertexList[iVertex].I2.w,
					&pSkinMesh.m_IW_VertexList[iVertex].W1.x,
					&pSkinMesh.m_IW_VertexList[iVertex].W1.y,
					&pSkinMesh.m_IW_VertexList[iVertex].W1.z,
					&pSkinMesh.m_IW_VertexList[iVertex].W1.w,
					&pSkinMesh.m_IW_VertexList[iVertex].W2.x,
					&pSkinMesh.m_IW_VertexList[iVertex].W2.y,
					&pSkinMesh.m_IW_VertexList[iVertex].W2.z,
					&pSkinMesh.m_IW_VertexList[iVertex].W2.w);
			}
			;
			for (int iIndex = 0; iIndex < iIndexSize; iIndex += 3)
			{
				m_Paser.GetNextLine();
				_stscanf_s(m_Paser.m_pBuffer, _T("%d %d %d"),
					&pSkinMesh._IndexList[iIndex + 0],
					&pSkinMesh._IndexList[iIndex + 1],
					&pSkinMesh._IndexList[iIndex + 2]);
			}

			CreateStructuredBuffer(m_pDevice, sizeof(PNCT_VERTEX),
									pSkinMesh._VertexList.size(),
									&pSkinMesh._VertexList.at(0),
									&m_pVertexBuffer);
			CreateStructuredBuffer(m_pDevice, sizeof(IW_VERTEX),
				pSkinMesh.m_IW_VertexList.size(),
				&pSkinMesh.m_IW_VertexList.at(0),
				&m_pIWBuffer);

			CreateStructuredBuffer(m_pDevice, sizeof(PNCT_VERTEX),
									pSkinMesh._VertexList.size(), NULL,
									&m_pBoneVertexBuffer);

			//"Creating buffer views..."
			CreateBufferSRV(m_pDevice, m_pVertexBuffer, &m_pVertexBufferSRV);
			CreateBufferSRV(m_pDevice, m_pIWBuffer, &m_pIWBufferSRV);
			CreateBufferUAV(m_pDevice, m_pBoneVertexBuffer, &m_pBoneVertexBufferUAV);

			//"Running Compute Shader..."
			ID3D11ShaderResourceView* aRViews[3] = { m_pVertexBufferSRV, m_pIWBufferSRV, m_pInvMatBufferSRV };
			RunComputeShader(m_pContext, m_pCS, 3, aRViews, NULL, NULL, 0, m_pBoneVertexBufferUAV, pSkinMesh._VertexList.size(), 1, 1);

			// Read back the result from GPU, verify its correctness against result computed by CPU
			{
				ID3D11Buffer* debugbuf = CreateAndCopyToBuf(m_pDevice, m_pContext, m_pBoneVertexBuffer);
				D3D11_MAPPED_SUBRESOURCE MappedResource;
				PNCT_VERTEX *p;
				m_pContext->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);

				// Set a break point here and put down the expression "p, 1024" in your watch window to see what has been written out by our CS
				// This is also a common trick to debug CS programs.
				p = (PNCT_VERTEX*)MappedResource.pData;

				for (int iCount = 0; iCount < pSkinMesh._VertexList.size(); iCount++)
				{
					pSkinMesh._VertexList[iCount].p = p[iCount].p;
				}
				// Verify that if Compute Shader has done right

				m_pContext->Unmap(debugbuf, 0);
				SAFE_RELEASE(debugbuf);

				SAFE_RELEASE(m_pVertexBuffer);
				SAFE_RELEASE(m_pIWBuffer);
				SAFE_RELEASE(m_pBoneVertexBuffer);

				SAFE_RELEASE(m_pVertexBufferSRV);
				SAFE_RELEASE(m_pIWBufferSRV);
				SAFE_RELEASE(m_pBoneVertexBufferUAV);

			}
		}

		pSkinObject->Init();
		pSkinObject->Create(m_pDevice, m_pContext, L"Test.hlsl");
	}
	SAFE_RELEASE(m_pInvMatBuffer);
	SAFE_RELEASE(m_pInvMatBufferSRV);

	m_Paser.CloseStream();

	return true;
}

//--------------------------------------------------------------------------------------
// Compile and create the CS
//--------------------------------------------------------------------------------------
HRESULT SkinFileLoader::CreateComputeShader(LPCWSTR pSrcFile, LPCSTR pFunctionName,
	ID3D11Device* pDevice, ID3D11ComputeShader** ppShaderOut)
{
	HRESULT hr;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG ) 
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
#ifdef USE_STRUCTURED_BUFFERS
		"USE_STRUCTURED_BUFFERS", "1",
#endif
		NULL, NULL
	};

	// We generally prefer to use the higher CS shader profile when possible as CS 5.0 is better performance on 11-class hardware
	LPCSTR pProfile = (pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";

	ID3DBlob* pErrorBlob = NULL;
	ID3DBlob* pBlob = NULL;
	hr = D3DX11CompileFromFile(pSrcFile, defines, NULL, pFunctionName, pProfile,
		dwShaderFlags, NULL, NULL, &pBlob, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

		SAFE_RELEASE(pErrorBlob);
		SAFE_RELEASE(pBlob);

		return hr;
	}

	hr = pDevice->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, ppShaderOut);

	SAFE_RELEASE(pErrorBlob);
	SAFE_RELEASE(pBlob);

	return hr;
}

//--------------------------------------------------------------------------------------
// Create Structured Buffer
//--------------------------------------------------------------------------------------
HRESULT SkinFileLoader::CreateStructuredBuffer(ID3D11Device* pDevice, UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBufOut)
{
	*ppBufOut = NULL;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = uElementSize * uCount;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = uElementSize;

	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		return pDevice->CreateBuffer(&desc, &InitData, ppBufOut);
	}
	else
		return pDevice->CreateBuffer(&desc, NULL, ppBufOut);
}

//--------------------------------------------------------------------------------------
// Create Shader Resource View for Structured or Raw Buffers
//--------------------------------------------------------------------------------------
HRESULT SkinFileLoader::CreateBufferSRV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	pBuffer->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
	}
	else
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer

			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			return E_INVALIDARG;
		}

	return pDevice->CreateShaderResourceView(pBuffer, &desc, ppSRVOut);
}

//--------------------------------------------------------------------------------------
// Create Unordered Access View for Structured or Raw Buffers
//-------------------------------------------------------------------------------------- 
HRESULT SkinFileLoader::CreateBufferUAV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	pBuffer->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer

		desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.NumElements = descBuf.ByteWidth / 4;
	}
	else
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer

			desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
			desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			return E_INVALIDARG;
		}

	return pDevice->CreateUnorderedAccessView(pBuffer, &desc, ppUAVOut);
}

//--------------------------------------------------------------------------------------
// Create a CPU accessible buffer and download the content of a GPU buffer into it
// This function is very useful for debugging CS programs
//-------------------------------------------------------------------------------------- 
ID3D11Buffer* SkinFileLoader::CreateAndCopyToBuf(ID3D11Device* pDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11Buffer* pBuffer)
{
	ID3D11Buffer* debugbuf = NULL;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	pBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	if (SUCCEEDED(pDevice->CreateBuffer(&desc, NULL, &debugbuf)))
	{
		pd3dImmediateContext->CopyResource(debugbuf, pBuffer);
	}

	return debugbuf;
}

//--------------------------------------------------------------------------------------
// Run CS
//-------------------------------------------------------------------------------------- 
void SkinFileLoader::RunComputeShader(ID3D11DeviceContext* pd3dImmediateContext,
	ID3D11ComputeShader* pComputeShader,
	UINT nNumViews, ID3D11ShaderResourceView** pShaderResourceViews,
	ID3D11Buffer* pCBCS, void* pCSData, DWORD dwNumDataBytes,
	ID3D11UnorderedAccessView* pUnorderedAccessView,
	UINT X, UINT Y, UINT Z)
{
	pd3dImmediateContext->CSSetShader(pComputeShader, NULL, 0);
	pd3dImmediateContext->CSSetShaderResources(0, nNumViews, pShaderResourceViews);
	pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &pUnorderedAccessView, NULL);
	if (pCBCS)
	{
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		pd3dImmediateContext->Map(pCBCS, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		memcpy(MappedResource.pData, pCSData, dwNumDataBytes);
		pd3dImmediateContext->Unmap(pCBCS, 0);
		ID3D11Buffer* ppCB[1] = { pCBCS };
		pd3dImmediateContext->CSSetConstantBuffers(0, 1, ppCB);
	}

	pd3dImmediateContext->Dispatch(X, Y, Z);

	pd3dImmediateContext->CSSetShader(NULL, NULL, 0);

	ID3D11UnorderedAccessView* ppUAViewNULL[1] = { NULL };
	pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, ppUAViewNULL, NULL);

	ID3D11ShaderResourceView* ppSRVNULL[2] = { NULL, NULL };
	pd3dImmediateContext->CSSetShaderResources(0, 2, ppSRVNULL);

	ID3D11Buffer* ppCBNULL[1] = { NULL };
	pd3dImmediateContext->CSSetConstantBuffers(0, 1, ppCBNULL);
}


SkinFileLoader::SkinFileLoader()
{
}


SkinFileLoader::~SkinFileLoader()
{
}
