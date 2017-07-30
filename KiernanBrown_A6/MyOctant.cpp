#include "MyOctant.h"
using namespace Simplex;

int Simplex::MyOctant::m_octantCount = 0;

void Simplex::MyOctant::Init()
{
	// Set our Mesh and Entity Managers
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	// Give this octant its id and increment our octantCount
	m_id = m_octantCount;
	m_octantCount++;
}

void Simplex::MyOctant::Release()
{
	// Clear Dimension Set for all entities
	for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		m_pEntityMngr->GetEntity(i)->ClearDimensionSet();
	}

	// Remove all ChildNodes
	RemoveChildNodes();

	// Set the Mesh and Entity Managers to be nullptrs
	m_pMeshMngr = nullptr;
	m_pEntityMngr = nullptr;

	// Get rid of our root, octantCount, and id
	m_octantCount = 0;
	m_id = 0;
	m_pRoot = nullptr;
}

Simplex::MyOctant::MyOctant(int a_level, int a_idealCount)
{
	Init();

	m_pRoot = this;
	std::vector<vector3> minMaxList;	// List of the min and max vectors for all rigid bodies

	// Set the min and max for our Octant bounding box, and add these entities to our root
	for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		minMaxList.push_back(m_pEntityMngr->GetRigidBody(i)->GetMinGlobal());
		minMaxList.push_back(m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal());
		m_pObjects.push_back(i);
	}

	// Make a new rigidbody using this list, which lets us get our min and max for our Octant bounding box
	MyRigidBody* referenceRB = new MyRigidBody(minMaxList);
	
	// The min and max for this octant are the MaxGlobal and MinGlobal of the RigidBody
	m_v3Max = referenceRB->GetMaxGlobal();
	m_v3Min = referenceRB->GetMinGlobal();

	// Delete the RigidBody and clear the list as they are no longer being used
	SafeDelete(referenceRB);
	minMaxList.clear();

	// Set the center, width, level, and idealCount
	m_v3Center = (m_v3Max + m_v3Min) / 2.0f;
	m_v3Width = m_v3Max - m_v3Min;
	m_level = a_level;
	m_idealCount = a_idealCount;

	// Build the rest of the Octree
	BuildTree();
}

Simplex::MyOctant::MyOctant(vector3 a_v3Min, vector3 a_v3Max, int a_level, int a_idealCount)
{
	Init();
	m_v3Min = a_v3Min;
	m_v3Max = a_v3Max;
	m_v3Center = (m_v3Max + m_v3Min) / 2.0f;
	m_v3Width = m_v3Max - m_v3Min;
	m_level = a_level;
	m_idealCount = a_idealCount;
}

Simplex::MyOctant::MyOctant(MyOctant const& other)
{
	Init();
	m_id = other.m_id;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;
	m_v3Center = other.m_v3Center;
	m_v3Width = other.m_v3Width;
	m_level = other.m_level;
	m_idealCount = other.m_idealCount;
}

MyOctant& Simplex::MyOctant::operator=(MyOctant const& other)
{
	if (this != &other)
	{
		Init();
		m_id = other.m_id;
		m_v3Min = other.m_v3Min;
		m_v3Max = other.m_v3Max;
		m_v3Center = other.m_v3Center;
		m_v3Width = other.m_v3Width;
		m_level = other.m_level;
		m_idealCount = other.m_idealCount;
	}
	return *this;
}

Simplex::MyOctant::~MyOctant(void)
{
	Release();
}

// Build the Octree
void Simplex::MyOctant::BuildTree(void)
{
	// If this isn't the last level, and we have at least the ideal count of objects, then we will AddChildNodes
	if (m_level > 0 && m_pObjects.size() >= m_idealCount)
	{
		// Create the child nodes
		AddChildNodes();
	}
}

void Simplex::MyOctant::AddChildNodes(void)
{
	// Create the child nodes
	m_pChildNodes.push_back(new MyOctant(vector3(m_v3Min.x, m_v3Center.y, m_v3Center.z), vector3(m_v3Center.x, m_v3Max.y, m_v3Max.z), m_level - 1, m_idealCount));	// Front Top Left
	m_pChildNodes.push_back(new MyOctant(m_v3Center, m_v3Max, m_level - 1, m_idealCount));																			// Front Top Right
	m_pChildNodes.push_back(new MyOctant(vector3(m_v3Min.x, m_v3Min.y, m_v3Center.z), vector3(m_v3Center.x, m_v3Center.y, m_v3Max.z), m_level - 1, m_idealCount));	// Front Bottom Left
	m_pChildNodes.push_back(new MyOctant(vector3(m_v3Center.x, m_v3Min.y, m_v3Center.z), vector3(m_v3Max.x, m_v3Center.y, m_v3Max.z), m_level - 1, m_idealCount));	// Front Bottom Right
	m_pChildNodes.push_back(new MyOctant(vector3(m_v3Min.x, m_v3Center.y, m_v3Min.z), vector3(m_v3Center.x, m_v3Max.y, m_v3Center.z), m_level - 1, m_idealCount));	// Back Top Left
	m_pChildNodes.push_back(new MyOctant(vector3(m_v3Center.x, m_v3Center.y, m_v3Min.z), vector3(m_v3Max.x, m_v3Max.y, m_v3Center.z), m_level - 1, m_idealCount));	// Back Top Right
	m_pChildNodes.push_back(new MyOctant(m_v3Min, m_v3Center, m_level - 1, m_idealCount));																			// Back Bottom Left
	m_pChildNodes.push_back(new MyOctant(vector3(m_v3Center.x, m_v3Min.y, m_v3Min.z), vector3(m_v3Max.x, m_v3Center.y, m_v3Center.z), m_level - 1, m_idealCount));	// Back Bottom Right

	for (int i = 0; i < m_pChildNodes.size(); i++)
	{
		// Set the root and parent nodes
		m_pChildNodes[i]->m_pRoot = this->m_pRoot;
		m_pChildNodes[i]->m_pParentNode = this;

		// Add the objects to this Octant
		m_pChildNodes[i]->AddObjects();

		// Continue building the tree
		m_pChildNodes[i]->BuildTree();

		// If this Octant is a leaf, we will go through its objects and add this dimension
		if (m_pChildNodes[i]->IsLeaf())
		{
			m_pChildNodes[i]->AddDimensions();
		}
	}
}

// Add objects to the Octant's object list if they are in the Octant
void Simplex::MyOctant::AddObjects(void)
{
	std::vector<uint> checkObjectList = m_pParentNode->m_pObjects;

	for (int i = 0; i < checkObjectList.size(); i++)
	{
		if (IsObjectInOctant(m_pEntityMngr->GetEntity(checkObjectList[i])))
		{
			m_pObjects.push_back(checkObjectList[i]);
		}
	}
}

// Add this Octant's dimension to each object this node contains
void Simplex::MyOctant::AddDimensions(void)
{
	for (int i = 0; i < m_pObjects.size(); i++)
	{
		m_pEntityMngr->GetEntity(m_pObjects[i])->AddDimension(m_id);
	}
}

// Display the entire Octree
void Simplex::MyOctant::Display(void)
{
	// Display this octant
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3Width), C_YELLOW);

	// Then loop through it's children and display them
	for (int i = 0; i < m_pChildNodes.size(); i++)
	{
		if (m_pChildNodes[i] != nullptr)
		{
			m_pChildNodes[i]->Display();
		}
	}
}

// Display the Octant with the specified index
void Simplex::MyOctant::Display(int a_index)
{
	// Display this octant if it has the specified index
	if (m_id == a_index)
	{
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3Width), C_YELLOW);
	}

	// Otherwise, loop through it's children and call display for them to find the right octant
	else
	{
		for (int i = 0; i < m_pChildNodes.size(); i++)
		{
			if (m_pChildNodes[i] != nullptr)
			{
				m_pChildNodes[i]->Display(a_index);
			}
		}
	}
}

// Return m_octantCount
int Simplex::MyOctant::GetOctantCount(void)
{
	return m_octantCount;
}

bool Simplex::MyOctant::IsLeaf(void)
{
	// If this Octant has no children, it is a leaf
	if (m_pChildNodes.size() == 0)
	{
		return true;
	}
	return false;
}

bool Simplex::MyOctant::IsObjectInOctant(MyEntity * a_object)
{
	// Get the min and max of the object
	vector3 objMin = a_object->GetRigidBody()->GetMinGlobal();
	vector3 objMax = a_object->GetRigidBody()->GetMaxGlobal();

	// Check the min, then the max
	// If the object's min/max is greater than this octant's min, and the object's min/max are less then this octant's max, then it is in the octant
	if (objMin.x > m_v3Min.x && objMin.x < m_v3Max.x)
	{
		if (objMin.y > m_v3Min.y && objMin.y < m_v3Max.y)
		{
			if (objMin.z > m_v3Min.z && objMin.z < m_v3Max.z)
			{
				// It is in the octant, return true
				return true;
			}
		}
	}
	if (objMax.x > m_v3Min.x && objMax.x < m_v3Max.x)
	{
		if (objMax.y > m_v3Min.y && objMax.y < m_v3Max.y)
		{
			if (objMax.z > m_v3Min.z && objMax.z < m_v3Max.z)
			{
				return true;
			}
		}
	}
	return false;
}

void Simplex::MyOctant::RemoveChildNodes(void)
{
	// Loop through the children of this node
	for (int i = 0; i < m_pChildNodes.size(); i++)
	{
		if (m_pChildNodes[i] != nullptr)
		{
			// Remove the children from this node
			m_pChildNodes[i]->RemoveChildNodes();

			// Then delete this node and set it's pointer to be a nullptr
			delete m_pChildNodes[i];
			m_pChildNodes[i] = nullptr;
		}
	}
}