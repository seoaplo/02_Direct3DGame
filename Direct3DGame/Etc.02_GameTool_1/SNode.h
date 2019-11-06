#pragma once
#include "SDxBasic.h"
#include "SDXShape.h"
#include "SBaseObj.h"

class SNode
{
public:
	DWORD	m_dwDepth;
	BOOL	m_IsLeaf;
	// �ٿ�� �ڽ�
	S_BOX	m_sBox;

	std::vector<SNode*>			m_ChildList;
	std::vector<SBaseObj*>		m_ObjectList;
	// ����� �ڳ� ���� �ε��� �� ��ġ
	std::vector<D3DXVECTOR3>	m_CornerList;
	std::vector<DWORD>			m_CornerIndex;

	// �ε��� ����
	std::vector<DWORD>   m_IndexData;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;
public:
	SNode();
	~SNode();
};

