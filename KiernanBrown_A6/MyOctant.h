/*----------------------------------------------
Programmer: Kiernan Brown (kwb6721@g.rit.edu)
Date: 07/2017
----------------------------------------------*/
#ifndef __MYOCTANT_H_
#define __MYOCTANT_H_

#include "MyEntityManager.h"

namespace Simplex
{

	// Octant Class
	class MyOctant
	{
		std::vector<uint> m_pObjects;				// IDs for objects in this Octant
		MyOctant* m_pParentNode = nullptr;			// Parent node for this octant
		std::vector<MyOctant*> m_pChildNodes;		// Child nodes for this octant
		vector3 m_v3Min;							// Min coordinates for our Bounding Box
		vector3 m_v3Max;							// Max coordinates for our Bounding Box
		vector3 m_v3Center;							// Center of this octant
		vector3 m_v3Width;							// Width of this octant
		MeshManager* m_pMeshMngr = nullptr;			// MeshManager
		MyEntityManager* m_pEntityMngr = nullptr;	// EntityManager
		matrix4 m_m4ToWorld = IDENTITY_M4;			// Matrix used for globalizing the octant
		int m_id;									// ID of the octant
		int m_level;								// The level of this Octant (0 is a leaf, highest number is the root)
		int m_idealCount;							// Ideal count for this octant
		static int m_octantCount;					// Number of octants
		MyOctant* m_pRoot = nullptr;				// Root octant
	public:
		// Init and Release
		void Init();
		void Release();
		
		// Constructor
		MyOctant(int a_level, int a_idealCount);									// Creates an octant given the number of levels and the ideal count, used for creating the root only
		MyOctant(vector3 a_v3Min, vector3 a_v3Max, int a_level, int a_idealCount);	// Creates an Octant node given a min, max, level of the node, and the ideal count

		// Copy Constructor
		MyOctant(MyOctant const& other);

		// Copy Assignment Operator
		MyOctant& operator=(MyOctant const& other);

		// Destructor
		~MyOctant(void);

		// Public Methods
		void BuildTree(void);							// Builds the Octant
		void AddChildNodes(void);						// Adds child nodes to the Octant node
		void AddObjects(void);							// Adds the objects to the Octant node
		void AddDimensions(void);						// Adds the dimensions to the objects
		void Display(void);								// Displays the entire Octant
		void Display(int a_index);						// Displays the specified Octant node
		int GetOctantCount(void);						// Returns the total number of octants
		bool IsLeaf(void);								// Returns true if this is a leaf
		bool IsObjectInOctant(MyEntity* a_object);		// Returns if the object is within this octant
		void RemoveChildNodes(void);					// Method to remove child nodes, used when releasing the root
	};
}
#endif //__MYOCTANT_H_