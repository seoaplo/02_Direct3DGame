#pragma once
#include "SDxBasic.h"
#include "SDXShape.h"

class SNode
{
public:
	DWORD	m_dwDepth;
	BOOL	m_IsLeaf;
	// �ٿ�� �ڽ�
	S_BOX	m_sBox;

	// ����� �ڳ� ���� �ε��� �� ��ġ
	std::vector<D3DXVECTOR3>	m_CornerList;
	std::vector<SNode*>			m_ChildList;
public:
	SNode();
	~SNode();
};

