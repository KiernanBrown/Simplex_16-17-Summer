#ifndef __MAP_H_
#define __MAP_H_

#include "MyEntity.h"

namespace Simplex
{

	class Map : public MyEntity
	{
		int m_width;							// Width of the map (in terms of MapNodes)
		int m_length;							// Length of the map (in terms of MapNodes)
		int m_height;							// Height of the map (in terms of MapNodes)
		String m_sMapName;						// Map Name
		SystemSingleton* m_pSystem = nullptr;	// SystemSingleton used for loading the map file
		vector3 m_v3NodeSize;					// Size of each MapNode

	public:
		// Constructor and Big 3
		Map(String a_sMapName, String a_sUniqueID = "NA");
		Map(Map const& other);
		Map& operator=(Map const& other);
		~Map();

		void Release();
		std::vector<MapNode*> m_pMapNodes;		// List of all MapNodes in the map
		void LoadMap();							// Loads in the map file of the map
		vector3 GetNodeSize();					// Returns the size of each MapNode
		MapNode* GetNode(vector2 a_v2Index);	// Returns the MapNode at the given index
		void SetNeighbors();					// Set's the neighbors of every MapNode in the map
	};

}

#endif //__MAP_H_