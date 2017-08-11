#ifndef __MAPNODE_H_
#define __MAPNODE_H_

#include "Definitions.h"

namespace Simplex
{

class MapNode
{
	vector2 m_v2Index;							// Index of this node in the map
	bool m_bWalkable;							// Can this map be walked on
	std::vector<MapNode*> m_pNeighborNodes;		// List of neighboring nodes
	MapNode* m_pParentNode;						// Parent node of this node
	float m_fTerrainCost;						// Terrain cost of this node
	int m_Height;

public:

	// Is this not in a list, in the open list, or in the closed list
	enum InList
	{
		None,
		Open,
		Closed
	};

	InList m_eInList;			// What list this node is in
	float m_fGoalDistance;		// Distance from start node to goal node if path goes through this node (TraveledDistance + Distance to End)
	float m_fTraveledDistance;	// Distance traveled from start node
	vector3 m_v3Center;			// Center of this node

	// Constructor and Big 3
	MapNode(vector2 a_v2Index, int a_Height, bool a_bWalkable);
	MapNode(MapNode const& other);
	MapNode& operator=(MapNode const& other);
	~MapNode();

	void Release();
	bool IsWalkable();								// Returns if this MapNode is walkable
	void SetWalkable(bool a_bWalkable);				// Sets the walkability of this MapNode
	void SetTerrainCost(float a_fTerrainCost);		// Sets the TerrainCost of this MapNode
	float GetTerrainCost();							// Returns the TerrainCost of this MapNode
	vector2 GetIndex();								// Returns the Index of this MapNode
	std::vector<MapNode*> GetNeighbors();			// Returns the Neighbors of this MapNode
	void SetParent(MapNode* a_pParentNode);			// Sets the Parent of this MapNode
	MapNode* GetParent();							// Returns the Parent of this MapNode
	vector3 GetCenter();							// Returns the Center of this MapNode
	void AddNeighbor(MapNode* a_pNeighbor);			// Adds a Neighbor to this MapNodes list of Neighbors
	void ClearNeighbors();							// Clears the Neighbor List
};

}

#endif //__MAPNODE_H_