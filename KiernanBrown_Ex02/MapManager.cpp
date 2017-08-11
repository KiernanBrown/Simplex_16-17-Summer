#include "MapManager.h"
using namespace Simplex;
//  MyEntityManager
Simplex::MapManager* Simplex::MapManager::m_pInstance = nullptr;

Simplex::MapManager::MapManager() { Init(); }

Simplex::MapManager::~MapManager() { Release(); }

MapManager & Simplex::MapManager::operator=(MapManager const & a_pOther) { return *this; }

void Simplex::MapManager::Release()
{
	// Release all maps
	for (int i = 0; i < m_uMapCount; i++)
	{
		Map* map = m_pMaps[i];
		SafeDelete(map);
	}

	// Reset our MapCount and clear our Maps list
	m_uMapCount = 0;
	m_pMaps.clear();
}

void Simplex::MapManager::Init()
{
	m_uMapCount = 0;
}

MapManager * Simplex::MapManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MapManager;
	}
	return m_pInstance;
}

void Simplex::MapManager::ReleaseInstance()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void Simplex::MapManager::AddMap(Map * a_pMap)
{
	m_pMaps.push_back(a_pMap);
	m_uMapCount++;
}

void Simplex::MapManager::SetCurrentMap(int a_index)
{
	if (a_index < m_uMapCount)
	{
		m_pCurrentMap = m_pMaps[a_index];
	}
}

void Simplex::MapManager::SetCurrentMap(Map * a_pMap)
{
	for (int i = 0; i < m_uMapCount; i++)
	{
		if (m_pMaps[i] == a_pMap)
		{
			m_pCurrentMap = m_pMaps[i];
		}
	}
}

Map * Simplex::MapManager::GetCurrentMap()
{
	return m_pCurrentMap;
}