/*----------------------------------------------
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2017/05
----------------------------------------------*/
#ifndef __MYMESH_H_
#define __MYMESH_H_

//#define USEBASICX
#ifdef USEBASICX
#include "BasicX\BasicX.h"
using namespace BasicX;
#else
#include "Simplex\Simplex.h"
using namespace Simplex;
#endif // USEBASICX

#include "SFML\Window.hpp"
#include "SFML\Graphics.hpp"
#include "SFML\OpenGL.hpp"

#include "ControllerConfiguration.h"
#include "imgui\ImGuiObject.h"

class MyMesh
{
	bool m_bBinded = false; //Binded flag
	uint m_uVertexCount = 0; //Number of vertices in this Mesh
	GLuint m_VAO = 0;			//OpenGL Vertex Array Object
	GLuint m_VBO = 0;			//OpenGL Vertex Array Object

	std::vector<vector3> m_lVertex; //Composed vertex array
	std::vector<vector3> m_lVertexPos;	//List of Vertices
	std::vector<vector3> m_lVertexCol;	//List of Colors

	ShaderManager* m_pShaderMngr = nullptr;	//Shader Manager

public:
	/*
	USAGE: Initialize the object's fields
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void Init(void);
	/*
	USAGE: Releases the object from memory
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void Release(void);
	/*
	USAGE: Constructor
	ARGUMENTS: ---
	OUTPUT: class object
	*/
	MyMesh();
	/*
	USAGE: Destructor
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	~MyMesh();
	/*
	USAGE: Copy Constructor -> Mesh does not allow to copy
	from other Mesh objects, create an empty Mesh
	and use the Instantiate method instead
	ARGUMENTS: class object (to copy)
	OUTPUT: class object
	*/
	MyMesh(MyMesh& other);
	/*
	USAGE: Copy Assignment Operator -> Mesh does not allow to copy
	from other SimpleMesh objects, use the Instantiate method instead.
	ARGUMENTS: class object (to copy)
	OUTPUT: class object
	*/
	MyMesh& operator=(MyMesh& other);
	/*
	USAGE: Swaps the contents of the object with another object's content
	ARGUMENTS: class object  (to swap with)
	OUTPUT: ---
	*/
	void Swap(MyMesh& other);
	/*
	USAGE: Completes the information missing to create the mesh
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void CompleteMesh(vector3 a_v3Color = vector3(1.0f, 0.0f, 0.0f));
	/*
	USAGE: Adds a new point to the vector of vertices
	ARGUMENTS:
	- vector3 a_v3Input -> vector input
	OUTPUT: ---
	*/
	void AddVertexPosition(vector3 a_v3Input);
	/*
	USAGE: Adds a new color to the vector of vertices
	ARGUMENTS:
	- vector3 a_v3Input -> vector input
	OUTPUT: ---
	*/
	void AddVertexColor(vector3 a_v3Input);
	/*
	USAGE: Compiles the Mesh for OpenGL 3.X use
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void CompileOpenGL3X(void);
	/*
	USAGE: Renders the mesh on the specified position by the
	provided camera view and projection
	ARGUMENTS:
	matrix4 a_mProjection -> Projection matrix
	matrix4 a_mView -> View matrix
	matrix4 a_mModel -> matrix of the model in the world
	OUTPUT: ---
	*/
	void Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel);
	/*
	USAGE: Adds a tri to the list points in the buffer to be compiled
	//C
	//| \
	//A--B
	//This will make the triang A->B->C
	ARGUMENTS:
	vector3 a_vBottomLeft (A)
	vector3 a_vBottomRight (B)
	vector3 a_vTopLeft (C)
	OUTPUT: ---
	*/
	void AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft);
	/*
	USAGE: Adds a quad to the list points in the buffer to be compiled
	//C--D
	//|  |
	//A--B
	//Will make the triang A->B->C and then the triang C->B->D
	ARGUMENTS:
	vector3 a_vBottomLeft (A)
	vector3 a_vBottomRight (B)
	vector3 a_vTopLeft (C)
	vector3 a_vTopRight (D)
	OUTPUT: ---
	*/
	void AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight);
	/*
	USAGE: Generates a cube
	ARGUMENTS:
	float a_fSize -> Size of each side
	vector3 a_v3Color -> Color of the mesh
	OUTPUT: ---
	*/
	void GenerateCube(float a_fSize, vector3 a_v3Color);
};
/*
USAGE:
ARGUMENTS: ---
OUTPUT: ---
*/
#endif //__MYMESH_H_