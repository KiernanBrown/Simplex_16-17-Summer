#include "Pathfinder.h"

Simplex::Pathfinder::Pathfinder(Map* a_pMap)
{
	// Give our pathfinder all of the map nodes from this map
	for (int i = 0; i < a_pMap->m_pMapNodes.size(); i++)
	{
		m_pMapNodes.push_back(a_pMap->m_pMapNodes[i]);
	}
	ResetMapNodes();
}

Simplex::Pathfinder::~Pathfinder()
{
	Release();
}

void Simplex::Pathfinder::Release()
{
	m_pMapNodes.clear();
	m_pOpenList.clear();
	m_pClosedList.clear();
}

void Simplex::Pathfinder::ResetMapNodes()
{
	// Clear the Open and Closed lists
	m_pOpenList.clear();
	m_pClosedList.clear();

	// Reset the TraveledDistance, GoalDistance, and InList for every node
	for (int i = 0; i < m_pMapNodes.size(); i++)
	{
		m_pMapNodes[i]->m_fTraveledDistance = 0;
		m_pMapNodes[i]->m_fGoalDistance = 0;	
		m_pMapNodes[i]->m_eInList = MapNode::None;
	}
}

Simplex::MapNode * Simplex::Pathfinder::FindClosestNode()
{
	MapNode* closestNode = m_pOpenList[0];
	currentOpenListIndex = 0;

	float smallestGoalDistance = FLT_MAX;

	// Find the closest node to the goal
	for (int i = 0; i < m_pOpenList.size(); i++)
	{
		if (m_pOpenList[i]->m_fGoalDistance < smallestGoalDistance)
		{
			closestNode = m_pOpenList[i];
			smallestGoalDistance = closestNode->m_fGoalDistance;
			currentOpenListIndex = i;
		}
	}

	return closestNode;
}

std::vector<Simplex::MapNode*> Simplex::Pathfinder::FindPath(MapNode * a_pStart, MapNode * a_pEnd)
{
	// We will only try to find a path if the start and end are different
	if (a_pStart == a_pEnd)
	{
		return std::vector<MapNode*>();
	}

	// Reset the MapNodes
	ResetMapNodes();

	// Set the GoalDistance of the start node and add it to the OpenList
	a_pStart->m_fGoalDistance = glm::distance(a_pStart->GetIndex(), a_pEnd->GetIndex());
	m_pOpenList.push_back(a_pStart);

	// If there are nodes in our OpenList and we haven't reached the end, keep checking nodes
	while (m_pOpenList.size() > 0)
	{
		// Loop through our open list and take the node with the smallest GoalDistance
		MapNode* currentNode = FindClosestNode();

		// If no node can be found, there is no path
		if (currentNode == nullptr)
		{
			break;
		}

		//std::cout << "CurrentNode Index " << currentNode->GetIndex().x << " " << currentNode->GetIndex().y << std::endl;

		// If the current node is our goal, we return the path we found
		if (currentNode == a_pEnd)
		{
			std::vector<MapNode*> path;

			// Add the end node to our path
			path.push_back(a_pEnd);

			MapNode* parent = a_pEnd->GetParent();

			// Trace back through the nodes using their parents to find our path in reverse
			while (parent != a_pStart)
			{
				path.push_back(parent);
				parent = parent->GetParent();
			}

			// Reverse the path and return it
			std::vector<MapNode*> finalPath;
			for (int i = path.size() - 1; i >= 0; i--)
			{
				finalPath.push_back(path[i]);
			}

			// Test output of path nodes
			/*for (int i = 0; i < finalPath.size(); i++)
			{
				std::cout << "Path Index " << i << ": (" << finalPath[i]->GetIndex().x << ", " << finalPath[i]->GetIndex().y << ")" << std::endl;
			}*/

			return finalPath;
		}

		// Otherwise, loop through the current node's neighbors
		for (int i = 0; i < currentNode->GetNeighbors().size(); i++)
		{
			MapNode* neighbor = currentNode->GetNeighbors()[i];

			// Make sure that the neighbor node is walkable and is not null
			if (neighbor == nullptr || !neighbor->IsWalkable())
			{
				continue;
			}

			// The traveledDistance is the current node's TraveledDistance + the neighbor's TerrainCost
			float traveledDistance = currentNode->m_fTraveledDistance + neighbor->GetTerrainCost();

			// The goalDistance is the distance from the neighbor to the end node
			float goalDistance = glm::distance(neighbor->GetIndex(), a_pEnd->GetIndex());

			// If the neighbor is not in a list
			if (neighbor->m_eInList == MapNode::None)
			{
				// Set the neighbor's TraveledDistance, GoalDistance, and ParentNode
				neighbor->m_fTraveledDistance = traveledDistance;
				neighbor->m_fGoalDistance = traveledDistance + goalDistance;
				neighbor->SetParent(currentNode);

				// Add this node to the OpenList
				neighbor->m_eInList = MapNode::Open;
				m_pOpenList.push_back(neighbor);
			}

			// If the neighbor is in a list, we will do the same steps but not add it to a list
			else
			{
				// First, check if traveledDistance than the neighbor's current TraveledDistance
				if (neighbor->m_fTraveledDistance > traveledDistance)
				{
					// Set the TraveledDistance, GoalDistance, and ParentNode
					neighbor->m_fTraveledDistance = traveledDistance;
					neighbor->m_fGoalDistance = traveledDistance + goalDistance;
					neighbor->SetParent(currentNode);
				}
			}
		}

		// Remove the current node from the open list and put it in the closed list
		m_pOpenList.erase(m_pOpenList.begin() + currentOpenListIndex);
		m_pClosedList.push_back(currentNode);
		currentNode->m_eInList = MapNode::Closed;
	}

	// No path could be found
	return std::vector<MapNode*>();
}
