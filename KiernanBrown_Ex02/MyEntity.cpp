#include "MyEntity.h"
using namespace Simplex;
std::map<String, MyEntity*> MyEntity::m_IDMap;
//  Accessors
Simplex::MySolver* Simplex::MyEntity::GetSolver(void) { return m_pSolver; }
bool Simplex::MyEntity::HasThisRigidBody(MyRigidBody* a_pRigidBody) { return m_pRigidBody == a_pRigidBody; }
Simplex::MyRigidBody::PRigidBody* Simplex::MyEntity::GetColliderArray(void) { return m_pRigidBody->GetColliderArray(); }
uint Simplex::MyEntity::GetCollidingCount(void) { return m_pRigidBody->GetCollidingCount(); }
matrix4 Simplex::MyEntity::GetModelMatrix(void){ return m_m4ToWorld; }
void Simplex::MyEntity::SetModelMatrix(matrix4 a_m4ToWorld)
{
	if (!m_bInMemory)
		return;

	m_m4ToWorld = a_m4ToWorld;
	m_pModel->SetModelMatrix(m_m4ToWorld);
	m_pRigidBody->SetModelMatrix(m_m4ToWorld);
	m_pSolver->SetPosition(vector3(m_m4ToWorld[3]));
}
Model* Simplex::MyEntity::GetModel(void){return m_pModel;}
MyRigidBody* Simplex::MyEntity::GetRigidBody(void){	return m_pRigidBody; }
bool Simplex::MyEntity::IsInitialized(void){ return m_bInMemory; }
String Simplex::MyEntity::GetUniqueID(void) { return m_sUniqueID; }
void Simplex::MyEntity::SetAxisVisible(bool a_bSetAxis) { m_bSetAxis = a_bSetAxis; }
void Simplex::MyEntity::SetPosition(vector3 a_v3Position) { if(m_pSolver) m_pSolver->SetPosition(a_v3Position); }
Simplex::vector3 Simplex::MyEntity::GetPosition(void)
{
	if (m_pSolver != nullptr)
		return m_pSolver->GetPosition();
	return vector3();
}

void Simplex::MyEntity::SetVelocity(vector3 a_v3Velocity) { if (m_pSolver) m_pSolver->SetVelocity(a_v3Velocity); }
Simplex::vector3 Simplex::MyEntity::GetVelocity(void)
{
	if (m_pSolver != nullptr)
		return m_pSolver->GetVelocity();
	return vector3();
}

void Simplex::MyEntity::SetMass(float a_fMass) { if (m_pSolver) m_pSolver->SetMass(a_fMass); }
float Simplex::MyEntity::GetMass(void)
{
	if (m_pSolver != nullptr)
		return m_pSolver->GetMass();
	return 1.0f;
}
//  MyEntity
void Simplex::MyEntity::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_pSystem = SystemSingleton::GetInstance();
	m_bInMemory = false;
	m_bSetAxis = false;
	m_pModel = nullptr;
	m_pRigidBody = nullptr;
	m_DimensionArray = nullptr;
	m_m4ToWorld = IDENTITY_M4;
	m_sUniqueID = "";
	m_nDimensionCount = 0;
	m_bUsePhysicsSolver = false;
	m_pSolver = nullptr;
}
void Simplex::MyEntity::Swap(MyEntity& other)
{
	m_bInMemory = false;
	std::swap(m_pModel, other.m_pModel);
	std::swap(m_pRigidBody, other.m_pRigidBody);
	std::swap(m_m4ToWorld, other.m_m4ToWorld);
	std::swap(m_pMeshMngr, other.m_pMeshMngr);
	std::swap(m_bInMemory, other.m_bInMemory);
	std::swap(m_sUniqueID, other.m_sUniqueID);
	std::swap(m_bSetAxis, other.m_bSetAxis);
	std::swap(m_nDimensionCount, other.m_nDimensionCount);
	std::swap(m_DimensionArray, other.m_DimensionArray);
	std::swap(m_pSolver, other.m_pSolver);
}
void Simplex::MyEntity::Release(void)
{
	m_pMeshMngr = nullptr;
	//it is not the job of the entity to release the model, 
	//it is for the mesh manager to do so.
	m_pModel = nullptr;
	if (m_DimensionArray)
	{
		delete[] m_DimensionArray;
		m_DimensionArray = nullptr;
	}
	SafeDelete(m_pRigidBody);
	SafeDelete(m_pSolver);
	m_IDMap.erase(m_sUniqueID);
}
//The big 3
Simplex::MyEntity::MyEntity(String a_sFileName, String a_sUniqueID)
{
	Init();
	m_pModel = new Model();
	m_pModel->Load(a_sFileName);
	//if the model is loaded
	if (m_pModel->GetName() != "")
	{
		GenUniqueID(a_sUniqueID);
		m_sUniqueID = a_sUniqueID;
		m_IDMap[a_sUniqueID] = this;
		m_pRigidBody = new MyRigidBody(m_pModel->GetVertexList()); //generate a rigid body
		m_bInMemory = true; //mark this entity as viable
	}
	m_pSolver = new MySolver();
}
Simplex::MyEntity::MyEntity(MyEntity const& other)
{
	m_bInMemory = other.m_bInMemory;
	m_pModel = other.m_pModel;
	//generate a new rigid body we do not share the same rigid body as we do the model
	m_pRigidBody = new MyRigidBody(m_pModel->GetVertexList()); 
	m_m4ToWorld = other.m_m4ToWorld;
	m_pMeshMngr = other.m_pMeshMngr;
	m_sUniqueID = other.m_sUniqueID;
	m_bSetAxis = other.m_bSetAxis;
	m_nDimensionCount = other.m_nDimensionCount;
	m_DimensionArray = other.m_DimensionArray;
	m_pSolver = new MySolver(*other.m_pSolver);
}
MyEntity& Simplex::MyEntity::operator=(MyEntity const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		MyEntity temp(other);
		Swap(temp);
	}
	return *this;
}
MyEntity::~MyEntity(){Release();}
//--- Methods
void Simplex::MyEntity::AddToRenderList(bool a_bDrawRigidBody)
{
	//if not in memory return
	if (!m_bInMemory)
		return;

	//draw model
	m_pModel->AddToRenderList();
	
	//draw rigid body
	if(a_bDrawRigidBody)
		m_pRigidBody->AddToRenderList();

	if (m_bSetAxis)
		m_pMeshMngr->AddAxisToRenderList(m_m4ToWorld);
}
MyEntity* Simplex::MyEntity::GetEntity(String a_sUniqueID)
{
	//look the entity based on the unique id
	auto entity = m_IDMap.find(a_sUniqueID);
	//if not found return nullptr, if found return it
	return entity == m_IDMap.end() ? nullptr : entity->second;
}
void Simplex::MyEntity::GenUniqueID(String& a_sUniqueID)
{
	static uint index = 0;
	String sName = a_sUniqueID;
	MyEntity* pEntity = GetEntity(a_sUniqueID);
	//while MyEntity exists keep changing name
	while (pEntity)
	{
		a_sUniqueID = sName + "_" + std::to_string(index);
		index++;
		pEntity = GetEntity(a_sUniqueID);
	}
	return;
}
void Simplex::MyEntity::AddDimension(uint a_uDimension)
{
	//we need to check that this dimension is not already allocated in the list
	if (IsInDimension(a_uDimension))
		return;//it is, so there is no need to add

	//insert the entry
	uint* pTemp;
	pTemp = new uint[m_nDimensionCount + 1];
	if(m_DimensionArray)
	{
		memcpy(pTemp, m_DimensionArray, sizeof(uint) * m_nDimensionCount);
		delete[] m_DimensionArray;
		m_DimensionArray = nullptr;
	}
	pTemp[m_nDimensionCount] = a_uDimension;
	m_DimensionArray = pTemp;

	++m_nDimensionCount;

	SortDimensions();
}
void Simplex::MyEntity::RemoveDimension(uint a_uDimension)
{
	//if there are no dimensions return
	if (m_nDimensionCount == 0)
		return;

	//we look one by one if its the one wanted
	for (uint i = 0; i < m_nDimensionCount; i++)
	{
		if (m_DimensionArray[i] == a_uDimension)
		{
			//if it is, then we swap it with the last one and then we pop
			std::swap(m_DimensionArray[i], m_DimensionArray[m_nDimensionCount - 1]);
			uint* pTemp;
			pTemp = new uint[m_nDimensionCount - 1];
			if (m_DimensionArray)
			{
				memcpy(pTemp, m_DimensionArray, sizeof(uint) * (m_nDimensionCount-1));
				delete[] m_DimensionArray;
				m_DimensionArray = nullptr;
			}
			m_DimensionArray = pTemp;
			
			--m_nDimensionCount;
			SortDimensions();
			return;
		}
	}
}
void Simplex::MyEntity::ClearDimensionSet(void)
{
	if (m_DimensionArray)
	{
		delete[] m_DimensionArray;
		m_DimensionArray = nullptr;
	}
	m_nDimensionCount = 0;
}
bool Simplex::MyEntity::IsInDimension(uint a_uDimension)
{
	//see if the entry is in the set
	for (uint i = 0; i < m_nDimensionCount; i++)
	{
		if (m_DimensionArray[i] == a_uDimension)
			return true;
	}
	return false;
}
bool Simplex::MyEntity::SharesDimension(MyEntity* const a_pOther)
{
	
	//special case: if there are no dimensions on either MyEntity
	//then they live in the special global dimension
	if (0 == m_nDimensionCount)
	{
		//if no spatial optimization all cases should fall here as every 
		//entity is by default, under the special global dimension only
		if(0 == a_pOther->m_nDimensionCount)
			return true;
	}

	//for each dimension on both Entities we check if there is a common dimension
	for (uint i = 0; i < m_nDimensionCount; ++i)
	{
		for (uint j = 0; j < a_pOther->m_nDimensionCount; j++)
		{
			if (m_DimensionArray[i] == a_pOther->m_DimensionArray[j])
				return true; //as soon as we find one we know they share dimensionality
		}
	}

	//could not find a common dimension
	return false;
}
bool Simplex::MyEntity::IsColliding(MyEntity* const other)
{
	//if not in memory return
	if (!m_bInMemory || !other->m_bInMemory)
		return true;

	//if the entities are not living in the same dimension
	//they are not colliding
	if (!SharesDimension(other))
		return false;

	return m_pRigidBody->IsColliding(other->GetRigidBody());
}
void Simplex::MyEntity::ClearCollisionList(void)
{
	m_pRigidBody->ClearCollidingList();
}
void Simplex::MyEntity::SortDimensions(void)
{
	std::sort(m_DimensionArray, m_DimensionArray + m_nDimensionCount);
}
void Simplex::MyEntity::ApplyForce(vector3 a_v3Force)
{
	m_pSolver->ApplyForce(a_v3Force);
}
void Simplex::MyEntity::Update(void)
{
	// Move along our path if we have one
	if (m_path != -1)
	{
		// LERP Along the path
		m_fPathTimerCount += m_pSystem->GetDeltaTime(m_uClock);									//get the delta time for our timer
		vector3 startPoint = m_pPathList[m_path]->GetCenter();									// startPoint is the starting point of the path we are on
		vector3 endPoint = m_pPathList[(m_path + 1)]->GetCenter();								// endPoint is the next point we are moving to 
		float pathPercentage = MapValue(m_fPathTimerCount, 0.0f, m_fPathTime, 0.0f, 1.0f);		// Percentage of the path we are done with

		// Calculate the current position
		vector3 v3CurrentPos = glm::lerp(startPoint, endPoint, pathPercentage);

		// Set our rotation
		glm::mat4x4 m4Rotation = glm::rotate(IDENTITY_M4, 0.0f, vector3(0, 1, 0));
		if (startPoint.z > endPoint.z)
		{
			m4Rotation = glm::rotate(IDENTITY_M4, 180.0f, vector3(0, 1, 0));
		}
		else if (startPoint.x > endPoint.x)
		{
			m4Rotation = glm::rotate(IDENTITY_M4, 270.0f, vector3(0, 1, 0));
		}
		else if (startPoint.x < endPoint.x)
		{
			m4Rotation = glm::rotate(IDENTITY_M4, 90.0f, vector3(0, 1, 0));
		}

		// When we finish this path, we reset the timer and go to the next path
		if (pathPercentage >= 1.0f)
		{
			// Update our current node
			SetCurrentNode(m_pPathList[(m_path + 1)]);

			// Increase our path count and reset the clock
			m_path++;
			m_fPathTimerCount = m_pSystem->GetDeltaTime(m_uClock);

			// If our path is greater than the size of our PathList - 1, we are done following the path
			if (m_path == m_pPathList.size() - 1)
			{
				m_path = -1;
				m_pPathList.clear();
				m_fPathTime = 0.0f;
			}
		}

		// Update the model's matrix
		m_m4ToWorld = glm::translate(v3CurrentPos) * m4Rotation;
		SetModelMatrix(m_m4ToWorld);
	}
	else if(m_pCurrentNode != nullptr)
	{
		m_m4ToWorld = glm::translate(m_pCurrentNode->GetCenter());
		SetModelMatrix(m_m4ToWorld);
	}

	/*if (m_bUsePhysicsSolver)
	{
		m_pSolver->Update();
		SetModelMatrix(glm::translate(m_pSolver->GetPosition()));
	}*/
}
void Simplex::MyEntity::ResolveCollision(MyEntity* a_pOther)
{
	if (m_bUsePhysicsSolver)
	{
		m_pSolver->ResolveCollision(a_pOther->GetSolver());
	}
}
void Simplex::MyEntity::UsePhysicsSolver(bool a_bUse)
{
	m_bUsePhysicsSolver = a_bUse;
}

void Simplex::MyEntity::FollowPath(std::vector<MapNode*> a_pPath, float a_fPathTime)
{
	if (a_pPath.size() > 0)
	{
		m_pPathList = a_pPath;
		m_fPathTime = a_fPathTime;
		m_path = 0;
		m_uClock = m_pSystem->GenClock();
	}
}

void Simplex::MyEntity::SetCurrentNode(MapNode * a_pMapNode)
{
	m_pCurrentNode = a_pMapNode;
}

MapNode * Simplex::MyEntity::GetCurrentNode()
{
	return m_pCurrentNode;
}
