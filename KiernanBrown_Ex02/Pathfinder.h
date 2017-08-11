#ifndef __PATHFINDER_H_
#define __PATHFINDER_H_

#include "Map.h"

namespace Simplex
{

	class Pathfinder
	{
		std::vector<MapNode*> m_pMapNodes;		// List of walkable MapNodes
		std::vector<MapNode*> m_pOpenList;		// List of MapNodes left to check
		std::vector<MapNode*> m_pClosedList;	// List of MapNodes that have been checked
		int currentOpenListIndex = 0;			// Index of the current node in the open list we are checking, used so we can remove it

	public:
		Pathfinder(Map* a_pMap);
		~Pathfinder();

		void Release();
		void ResetMapNodes();					// Reset our MapNodes

		// Method that returns the node with the smallest GoalDistance
		MapNode* FindClosestNode();

		// Method that returns a list of MapNodes with a path from the start node to the end node
		std::vector<MapNode*> FindPath(MapNode* a_pStart, MapNode* a_pEnd);
	};

}

#endif //__PATHFINDER_H_