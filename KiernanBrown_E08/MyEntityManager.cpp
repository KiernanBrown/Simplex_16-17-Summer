#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
	// Initialize the values for the EntityManager
	m_uEntityCount = 0;
	m_entityList.clear();
}
void MyEntityManager::Release(void)
{
	// Loop through the entity list and SafeDelete them
	for (int i = 0; i < m_uEntityCount; i++)
	{
		SafeDelete(m_entityList[i]);
	}

	// Clear the list
	m_uEntityCount = 0;
	m_entityList.clear();
}
MyEntityManager* MyEntityManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void MyEntityManager::ReleaseInstance()
{
	// Release the instance, if we have an instance
	if (m_pInstance != nullptr)
	{
		// Delete the instance and set it to be a nullptr
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	// Loop to find the entity with the specified UniqueID and return the index
	for (int i = 0; i < m_uEntityCount; i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			return i;
		}
	}
	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	// If the index is out of bounds, set it to be the last index
	if (a_uIndex > m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	// Return the model of the entity at the specified index
	return m_entityList[a_uIndex]->GetModel();
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	// Loop to find the entity with the specified UniqueID and return the model
	for (int i = 0; i < m_uEntityCount; i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			return m_entityList[i]->GetModel();
		}
	}
	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	// If the index is out of bounds, set it to be the last index
	if (a_uIndex > m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	// Return the RigidBody of the entity at the specified index
	return m_entityList[a_uIndex]->GetRigidBody();
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	// Loop to find the entity with the specified UniqueID and return the RigidBody
	for (int i = 0; i < m_uEntityCount; i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			return m_entityList[i]->GetRigidBody();
		}
	}
	return nullptr;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	// If the index is out of bounds, set it to be the last index
	if (a_uIndex > m_uEntityCount)
	{
		a_uIndex = m_uEntityCount - 1;
	}

	// Return the ModelMatrix of the entity at the specified index
	return m_entityList[a_uIndex]->GetModelMatrix();
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	// Loop to find the entity with the specified UniqueID and return the ModelMatrix
	for (int i = 0; i < m_uEntityCount; i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			return m_entityList[i]->GetModelMatrix();
		}
	}
	return IDENTITY_M4;
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	// Loop to find the entity with the specified UniqueID and set the ModelMatrix
	for (int i = 0; i < m_uEntityCount; i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			m_entityList[i]->SetModelMatrix(a_m4ToWorld);
		}
	}
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	// If the index is out of bounds, we will use the last index
	if (a_uIndex > m_uEntityCount)
	{
		m_entityList[m_uEntityCount - 1]->SetModelMatrix(a_m4ToWorld);
	}

	// Otherwise, set the ModelMatrix for the entity at the specified index
	else
	{
		m_entityList[a_uIndex]->SetModelMatrix(a_m4ToWorld);
	}
}
//The big 3
MyEntityManager::MyEntityManager() { Init(); }
MyEntityManager::MyEntityManager(MyEntityManager const& other) { }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager() { Release(); };
// other methods
void Simplex::MyEntityManager::Update(void)
{
	// Check the collisions for all entites
	// First loop gives the first entity, second loop checks all entities in the list with a higher index
	for (int i = 0; i < m_uEntityCount; i++)
	{
		for (int j = i + 1; j < m_uEntityCount; j++)
		{
			m_entityList[i]->IsColliding(m_entityList[j]);
		}
	}
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	// Add the entity to the list and increase our EntityCount
	MyEntity* entity = new MyEntity(a_sFileName, a_sUniqueID);
	m_entityList.push_back(entity);
	m_uEntityCount++;
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	// Remove the entity at the specified index from the list and decrease our EntityCount
	m_entityList.erase(m_entityList.begin() + a_uIndex);
	m_uEntityCount--;
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	// Loop to find the entity with the specified UniqueID, remove it from the list, and decrease our EntityCount
	for (int i = 0; i < m_uEntityCount; i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			m_entityList.erase(m_entityList.begin() + i);
			m_uEntityCount--;
		}
	}
}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	// Return the UniqueID of the entity at the specified index
	return m_entityList[a_uIndex]->GetUniqueID();
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	// Return the Entity at the specified index
	return m_entityList[a_uIndex];
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	// If the index is out of bounds, we add all entities to the render list
	if (a_uIndex > m_uEntityCount)
	{
		for (int i = 0; i < m_uEntityCount; i++)
		{
			m_entityList[i]->AddToRenderList(a_bRigidBody);
		}
	}

	// Otherise, add the entity at the specified index to the RenderList
	else
	{
		m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
	}
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	// Loop to find the entity with the specified UniqueID and add it to the RenderList
	for (int i = 0; i < m_uEntityCount; i++)
	{
		if (m_entityList[i]->GetUniqueID() == a_sUniqueID)
		{
			m_entityList[i]->AddToRenderList(a_bRigidBody);
		}
	}
}