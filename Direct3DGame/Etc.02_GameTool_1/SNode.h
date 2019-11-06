#pragma once
#include "SDxBasic.h"
#include "SDXShape.h"
#include "SBaseObj.h"

class SNode
{
public:
	DWORD	m_dwDepth;
	BOOL	m_IsLeaf;
	// 바운딩 박스
	S_BOX	m_sBox;

	std::vector<SNode*>			m_ChildList;
	std::vector<SBaseObj*>		m_ObjectList;
	// 노드의 코너 정점 인덱스 및 위치
	std::vector<D3DXVECTOR3>	m_CornerList;
	std::vector<DWORD>			m_CornerIndex;

	// 인덱스 버퍼
	std::vector<DWORD>   m_IndexData;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;
public:
	SNode();
	~SNode();
};

