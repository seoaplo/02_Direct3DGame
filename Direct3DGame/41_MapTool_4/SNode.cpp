#include "SNode.h"



SNode::SNode()
{
	
}


SNode::~SNode()
{
	m_ChildList.clear();
	m_ObjectList.clear();
	m_CornerList.clear();
	m_CornerIndex.clear();

	m_IndexData.clear();
}