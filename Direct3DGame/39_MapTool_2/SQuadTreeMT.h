#pragma once
#include "SQuadTreeMap.h"
class SQuadTreeMT : public SQuadTreeMap
{

public:
	bool Build(SMap* pMap, int iNodeColl, int iNodeRow, int iCellDistance, int iCellSize) ;
	void CreateBuffer(SNode* pNode)override;
	void CreateIndexBuffer(SNode* pNode)override;
	void CreateTextureVertexBuffer(SNode* pNode);
public:
	SQuadTreeMT();
	virtual ~SQuadTreeMT();
};

