#include "MapNode.h"
using namespace Simplex;


Simplex::MapNode::MapNode(vector2 a_v2Index, int a_Height, bool a_bWalkable)
{
	m_v2Index = a_v2Index;
	m_Height = a_Height;
	m_bWalkable = a_bWalkable;
	m_fTerrainCost = 1;
	m_eInList = MapNode::None;
}

Simplex::MapNode::MapNode(MapNode const & other)
{
	m_v2Index = other.m_v2Index;
	m_Height = other.m_Height;
	m_bWalkable = other.m_bWalkable;
	m_fTerrainCost = other.m_fTerrainCost;
	m_eInList = other.m_eInList;
}

MapNode & Simplex::MapNode::operator=(MapNode const & other)
{
	if (this != &other)
	{
		m_v2Index = other.m_v2Index;
		m_Height = other.m_Height;
		m_bWalkable = other.m_bWalkable;
		m_fTerrainCost = other.m_fTerrainCost;
		m_eInList = other.m_eInList;
	}
	return *this;
}

Simplex::MapNode::~MapNode()
{
	Release();
}

void Simplex::MapNode::Release()
{
	ClearNeighbors();
	m_pParentNode = nullptr;
}

bool Simplex::MapNode::IsWalkable()
{
	return m_bWalkable;
}

void Simplex::MapNode::SetWalkable(bool a_bWalkable)
{
	m_bWalkable = a_bWalkable;
}

void Simplex::MapNode::SetTerrainCost(float a_fTerrainCost)
{
	m_fTerrainCost = a_fTerrainCost;
}

float Simplex::MapNode::GetTerrainCost()
{
	return m_fTerrainCost;
}

Simplex::vector2 Simplex::MapNode::GetIndex()
{
	return m_v2Index;
}

std::vector<Simplex::MapNode*> Simplex::MapNode::GetNeighbors()
{
	return m_pNeighborNodes;
}

void Simplex::MapNode::SetParent(MapNode * a_pParentNode)
{
	m_pParentNode = a_pParentNode;
}

Simplex::MapNode * Simplex::MapNode::GetParent()
{
	return m_pParentNode;
}

Simplex::vector3 Simplex::MapNode::GetCenter()
{
	return m_v3Center;
}

void Simplex::MapNode::AddNeighbor(MapNode * a_pNeighbor)
{
	m_pNeighborNodes.push_back(a_pNeighbor);
}

void Simplex::MapNode::ClearNeighbors()
{
	m_pNeighborNodes.clear();
}
