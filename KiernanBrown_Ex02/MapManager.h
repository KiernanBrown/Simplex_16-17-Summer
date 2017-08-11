#ifndef __MAPMANAGER_H_
#define __MAPMANAGER_H_

#include "Map.h"

namespace Simplex
{
	// Singleton MapManager class to Manage Maps
	class MapManager
	{
		uint m_uMapCount = 0;			// Number of maps in the list
		std::vector<Map*> m_pMaps;		// List of all Maps
		static MapManager* m_pInstance; // Singleton pointer
		Map* m_pCurrentMap = nullptr;	// Map we are currently using

		MapManager();
		~MapManager();
		MapManager(MapManager const& a_pOther);				// Copy Constructor
		MapManager& operator=(MapManager const& a_pOther);	// Copy Assignment Operator
		void Release();
		void Init();

	public:
		static MapManager* GetInstance();	// Returns the instance of the MapManager
		static void ReleaseInstance();		// Releases the instance
		void AddMap(Map* a_pMap);			// Add a map to our list of maps
		void SetCurrentMap(int a_index);	// Set our current map to the map at the specified index
		void SetCurrentMap(Map* a_pMap);	// Set our current map to the specified map
		Map* GetCurrentMap();				// Return our current map
	};
}

#endif //__MAPMANAGER_H_