#include "MyRigidBody.h"
using namespace Simplex;
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = false;
	m_bVisibleOBB = true;
	m_bVisibleARBB = false;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& a_pOther)
{
	std::swap(m_pMeshMngr, a_pOther.m_pMeshMngr);
	std::swap(m_bVisibleBS, a_pOther.m_bVisibleBS);
	std::swap(m_bVisibleOBB, a_pOther.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, a_pOther.m_bVisibleARBB);

	std::swap(m_fRadius, a_pOther.m_fRadius);

	std::swap(m_v3ColorColliding, a_pOther.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, a_pOther.m_v3ColorNotColliding);

	std::swap(m_v3Center, a_pOther.m_v3Center);
	std::swap(m_v3MinL, a_pOther.m_v3MinL);
	std::swap(m_v3MaxL, a_pOther.m_v3MaxL);

	std::swap(m_v3MinG, a_pOther.m_v3MinG);
	std::swap(m_v3MaxG, a_pOther.m_v3MaxG);

	std::swap(m_v3HalfWidth, a_pOther.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, a_pOther.m_v3ARBBSize);

	std::swap(m_m4ToWorld, a_pOther.m_m4ToWorld);

	std::swap(m_CollidingRBSet, a_pOther.m_CollidingRBSet);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	//Calculate the 8 corners of the cube
	//Back square
	m_v3Corners[0] = m_v3MinL;
	m_v3Corners[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	m_v3Corners[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	m_v3Corners[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);

	//Front square
	m_v3Corners[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	m_v3Corners[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	m_v3Corners[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	m_v3Corners[7] = m_v3MaxL;

	//Place them in world space
	for (uint uIndex = 0; uIndex < 8; ++uIndex)
	{
		m_v3Corners[uIndex] = vector3(m_m4ToWorld * vector4(m_v3Corners[uIndex], 1.0f));
	}

	//Identify the max and min as the first corner
	m_v3MaxG = m_v3MinG = m_v3Corners[0];

	//get the new max and min for the global box
	for (uint i = 1; i < 8; ++i)
	{
		if (m_v3MaxG.x < m_v3Corners[i].x) m_v3MaxG.x = m_v3Corners[i].x;
		else if (m_v3MinG.x > m_v3Corners[i].x) m_v3MinG.x = m_v3Corners[i].x;

		if (m_v3MaxG.y < m_v3Corners[i].y) m_v3MaxG.y = m_v3Corners[i].y;
		else if (m_v3MinG.y > m_v3Corners[i].y) m_v3MinG.y = m_v3Corners[i].y;

		if (m_v3MaxG.z < m_v3Corners[i].z) m_v3MaxG.z = m_v3Corners[i].z;
		else if (m_v3MinG.z > m_v3Corners[i].z) m_v3MinG.z = m_v3Corners[i].z;
	}

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3Center, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& a_pOther)
{
	m_pMeshMngr = a_pOther.m_pMeshMngr;

	m_bVisibleBS = a_pOther.m_bVisibleBS;
	m_bVisibleOBB = a_pOther.m_bVisibleOBB;
	m_bVisibleARBB = a_pOther.m_bVisibleARBB;

	m_fRadius = a_pOther.m_fRadius;

	m_v3ColorColliding = a_pOther.m_v3ColorColliding;
	m_v3ColorNotColliding = a_pOther.m_v3ColorNotColliding;

	m_v3Center = a_pOther.m_v3Center;
	m_v3MinL = a_pOther.m_v3MinL;
	m_v3MaxL = a_pOther.m_v3MaxL;

	m_v3MinG = a_pOther.m_v3MinG;
	m_v3MaxG = a_pOther.m_v3MaxG;

	m_v3HalfWidth = a_pOther.m_v3HalfWidth;
	m_v3ARBBSize = a_pOther.m_v3ARBBSize;

	m_m4ToWorld = a_pOther.m_m4ToWorld;

	m_CollidingRBSet = a_pOther.m_CollidingRBSet;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& a_pOther)
{
	if (this != &a_pOther)
	{
		Release();
		Init();
		MyRigidBody temp(a_pOther);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };
//--- a_pOther Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* a_pOther)
{
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	auto element = m_CollidingRBSet.find(a_pOther);
	if (element != m_CollidingRBSet.end())
		return;
	// we couldn't find the object so add it
	m_CollidingRBSet.insert(a_pOther);
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* a_pOther)
{
	m_CollidingRBSet.erase(a_pOther);
}
void MyRigidBody::ClearCollidingList(void)
{
	m_CollidingRBSet.clear();
}
bool MyRigidBody::IsColliding(MyRigidBody* const a_pOther)
{
	//check if spheres are colliding as pre-test
	bool bColliding = (glm::distance(GetCenterGlobal(), a_pOther->GetCenterGlobal()) < m_fRadius + a_pOther->m_fRadius);
	
	//if they are colliding check the SAT
	if (bColliding)
	{
		if(SAT(a_pOther) != eSATResults::SAT_NONE)
			bColliding = false;// reset to false
	}

	if (bColliding) //they are colliding
	{
		this->AddCollisionWith(a_pOther);
		a_pOther->AddCollisionWith(this);
	}
	else //they are not colliding
	{
		this->RemoveCollisionWith(a_pOther);
		a_pOther->RemoveCollisionWith(this);
	}

	return bColliding;
}
void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}

uint MyRigidBody::SAT(MyRigidBody* const a_pOther)
{
	std::vector<vector3> axes;	// 0-2 are the local axes of this RigidBody, 3-5 are the local axes of the other RigidBody, 6-14 are the cross products

	// Get the local axes of the RigidBodies
	axes.push_back(glm::normalize(m_v3Corners[1] - m_v3Corners[0]));						// Ax axis
	axes.push_back(glm::normalize(m_v3Corners[2] - m_v3Corners[0]));						// Ay axis
	axes.push_back(glm::normalize(m_v3Corners[4] - m_v3Corners[0]));						// Az axis
	axes.push_back(glm::normalize(a_pOther->m_v3Corners[1] - a_pOther->m_v3Corners[0]));	// Bx axis
	axes.push_back(glm::normalize(a_pOther->m_v3Corners[2] - a_pOther->m_v3Corners[0]));	// By axis
	axes.push_back(glm::normalize(a_pOther->m_v3Corners[4] - a_pOther->m_v3Corners[0]));	// Bz axis

	// Floats used to check for intersections in the projections
	float min1;
	float max1;
	float min2;
	float max2;

	// Loop to check the local axes of both RigidBodies
	for (int i = 0; i < 6; i++)
	{
		// Get the intervals for both RigidBodies
		GetIntervals(m_v3Corners, axes[i], min1, max1);
		GetIntervals(a_pOther->m_v3Corners, axes[i], min2, max2);

		// If max1 < min2 or max < min2, we have a separating plane
		if (max1 < min2 || max2 < min1)
		{
			// Return the axis that is separating
			switch (i)
			{
				case 0: return eSATResults::SAT_AX;
					break;
				case 1: return eSATResults::SAT_AY;
					break;
				case 2: return eSATResults::SAT_AZ;
					break;
				case 3: return eSATResults::SAT_BX;
					break;
				case 4: return eSATResults::SAT_BY;
					break;
				case 5: return eSATResults::SAT_BZ;
					break;
			}
		}
	}

	// If we have not found a separating axis yet, check the cross products
	// We will test these one at a time so we don't have to do all of the cross products if we find a separating axis early

	vector3 axis; // Vector that we will use as a start

	// Test Ax x Bx
	// Get the cross product of Ax and Bx
	axis = glm::cross(axes[0], axes[3]);

	// Check to see if the vector is not a Zero Vector
	// If it is a zero vector, we will not have a separating plane, so we don't have to check this
	if (axis != vector3(0.0f, 0.0f, 0.0f))
	{
		axes.push_back(glm::normalize(axis));
		GetIntervals(m_v3Corners, axes[6], min1, max1);
		GetIntervals(a_pOther->m_v3Corners, axes[6], min2, max2);
		if (max1 < min2 || max2 < min1)
		{
			return eSATResults::SAT_AXxBX;
		}
	}

	// The steps detailed above are done for each of the cross products
	// Test Ax x By
	axis = glm::cross(axes[0], axes[4]);
	if (axis != vector3(0.0f, 0.0f, 0.0f))
	{
		axes.push_back(glm::normalize(axis));
		GetIntervals(m_v3Corners, axes[7], min1, max1);
		GetIntervals(a_pOther->m_v3Corners, axes[7], min2, max2);
		if (max1 < min2 || max2 < min1)
		{
			return eSATResults::SAT_AXxBY;
		}
	}

	// Test Ax x Bz
	axis = glm::cross(axes[0], axes[5]);
	if (axis != vector3(0.0f, 0.0f, 0.0f))
	{
		axes.push_back(glm::normalize(axis));
		if (axis != vector3(0.0f, 0.0f, 0.0f))
		{
			axes.push_back(glm::normalize(axis));
			GetIntervals(m_v3Corners, axes[8], min1, max1);
			GetIntervals(a_pOther->m_v3Corners, axes[8], min2, max2);
			if (max1 < min2 || max2 < min1)
			{
				return eSATResults::SAT_AXxBZ;
			}
		}
	}

	// Test Ay x Bx
	axis = glm::cross(axes[1], axes[3]);
	if (axis != vector3(0.0f, 0.0f, 0.0f))
	{
		axes.push_back(glm::normalize(axis));
		GetIntervals(m_v3Corners, axes[9], min1, max1);
		GetIntervals(a_pOther->m_v3Corners, axes[9], min2, max2);
		if (max1 < min2 || max2 < min1)
		{
			return eSATResults::SAT_AYxBX;
		}
	}

	// Test Ay x By
	axis = glm::cross(axes[1], axes[4]);
	if (axis != vector3(0.0f, 0.0f, 0.0f))
	{
		axes.push_back(glm::normalize(axis));
		GetIntervals(m_v3Corners, axes[10], min1, max1);
		GetIntervals(a_pOther->m_v3Corners, axes[10], min2, max2);
		if (max1 < min2 || max2 < min1)
		{
			return eSATResults::SAT_AYxBY;
		}
	}

	// Test Ay x Bz
	axis = glm::cross(axes[1], axes[5]);
	if (axis != vector3(0.0f, 0.0f, 0.0f))
	{
		axes.push_back(glm::normalize(axis));
		GetIntervals(m_v3Corners, axes[11], min1, max1);
		GetIntervals(a_pOther->m_v3Corners, axes[11], min2, max2);
		if (max1 < min2 || max2 < min1)
		{
			return eSATResults::SAT_AYxBZ;
		}
	}

	// Test Az x Bx
	axis = glm::cross(axes[2], axes[3]);
	if (axis != vector3(0.0f, 0.0f, 0.0f))
	{
		axes.push_back(glm::normalize(axis));
		GetIntervals(m_v3Corners, axes[12], min1, max1);
		GetIntervals(a_pOther->m_v3Corners, axes[12], min2, max2);
		if (max1 < min2 || max2 < min1)
		{
			return eSATResults::SAT_AZxBX;
		}
	}

	// Test Az x By
	axis = glm::cross(axes[2], axes[4]);
	if (axis != vector3(0.0f, 0.0f, 0.0f))
	{
		axes.push_back(glm::normalize(axis));
		GetIntervals(m_v3Corners, axes[13], min1, max1);
		GetIntervals(a_pOther->m_v3Corners, axes[13], min2, max2);
		if (max1 < min2 || max2 < min1)
		{
			return eSATResults::SAT_AZxBY;
		}
	}

	// Test Az x Bz
	axis = glm::cross(axes[2], axes[5]);
	if (axis != vector3(0.0f, 0.0f, 0.0f))
	{
		axes.push_back(glm::normalize(axis));
		GetIntervals(m_v3Corners, axes[14], min1, max1);
		GetIntervals(a_pOther->m_v3Corners, axes[14], min2, max2);
		if (max1 < min2 || max2 < min1)
		{
			return eSATResults::SAT_AZxBZ;
		}
	}

	// If there cannot be a separating axis found, then we are colliding
	return eSATResults::SAT_NONE;
}

void Simplex::MyRigidBody::GetIntervals(vector3* corners, vector3 axis, float& min, float& max)
{
	// Reset the min and max given
	min = 9000;
	max = -9000;

	for (int i = 0; i < 8; i++)
	{
		// Get the dot products of this axes and each of the corners of this OBB and the other OBB
		float dotProduct = glm::dot(axis, corners[i]);

		// Set our mins and maxes using these dot products
		min = glm::min(min, dotProduct);
		max = glm::max(max, dotProduct);
	}
}
