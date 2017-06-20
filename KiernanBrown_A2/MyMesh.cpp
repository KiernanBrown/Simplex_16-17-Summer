#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triang A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triang A->B->C and then the triang C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Create the top and bottom center points
	vector3 centerTop(0, a_fHeight / 2.0f, 0);
	vector3 centerBottom(0, - a_fHeight / 2.0f, 0);

	// Create the points for the base
	vector3* basePoints = new vector3[a_nSubdivisions];
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// Find the angle that we are using to make the point
		float angle = 2 * glm::pi<float>() * (float)i / a_nSubdivisions;

		// Create the point with the x value being the radius times the cos of the angle, and the z value being the radius times the sin of the angle
		basePoints[i] = vector3(a_fRadius * cos(angle), centerBottom.y, a_fRadius * sin(angle));
	}

	// Loop to create the triangles that go to the top and bottom center points
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i != a_nSubdivisions - 1)
		{
			AddTri(basePoints[i], basePoints[i + 1], centerTop);
			AddTri(basePoints[i + 1], basePoints[i], centerBottom);
		}
		else
		{
			AddTri(basePoints[i], basePoints[0], centerTop);
			AddTri(basePoints[0], basePoints[i], centerBottom);
		}
	}

	// Delete the arrays of points as they are no longer needed
	delete[] basePoints;

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Create the top and bottom center points
	vector3 centerTop(0, a_fHeight / 2.0f, 0);
	vector3 centerBottom(0, -a_fHeight / 2.0f, 0);

	// Create the points for the bottom and top of the cylinder
	vector3* bottomPoints = new vector3[a_nSubdivisions];
	vector3* topPoints = new vector3[a_nSubdivisions];
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// Find the angle that we are using to make the point
		float angle = 2 * glm::pi<float>() * (float)i / a_nSubdivisions;

		// Create the two points with the x values being the radius times the cos of the angle, and the z values being the radius times the sin of the angle
		bottomPoints[i] = vector3(a_fRadius * cos(angle), centerBottom.y, a_fRadius * sin(angle));
		topPoints[i] = vector3(a_fRadius * cos(angle), centerTop.y, a_fRadius * sin(angle));
	}

	// Loop to create the triangles that go to the top and bottom center points
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i != a_nSubdivisions - 1)
		{
			AddTri(topPoints[i + 1], topPoints[i], centerTop);
			AddTri(bottomPoints[i], bottomPoints[i + 1], centerBottom);
		}
		else
		{
			AddTri(topPoints[0], topPoints[i], centerTop);
			AddTri(bottomPoints[i], bottomPoints[0], centerBottom);
		}
	}

	// Create the quads for the side of the cylinder
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i != a_nSubdivisions - 1)
		{
			AddQuad(bottomPoints[i + 1], bottomPoints[i], topPoints[i + 1], topPoints[i]);
		}
		else
		{
			AddQuad(bottomPoints[0], bottomPoints[i], topPoints[0], topPoints[i]);
		}
	}

	// Delete the arrays of points as they are no longer needed
	delete[] bottomPoints;
	delete[] topPoints;

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Create the top and bottom center points
	// These points are not going to be used to make quads or triangles, just for reference
	vector3 centerTop(0, a_fHeight / 2.0f, 0);
	vector3 centerBottom(0, -a_fHeight / 2.0f, 0);

	// Create the points for the bottom and top of the outer ring of the tube
	vector3* bottomOuterPoints = new vector3[a_nSubdivisions];
	vector3* topOuterPoints = new vector3[a_nSubdivisions];
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// Find the angle that we are using to make the point
		float angle = 2 * glm::pi<float>() * (float)i / a_nSubdivisions;

		// Create the two points with the x values being the outer radius times the cos of the angle, and the z values being the outer radius times the sin of the angle
		bottomOuterPoints[i] = vector3(a_fOuterRadius * cos(angle), centerBottom.y, a_fOuterRadius * sin(angle));
		topOuterPoints[i] = vector3(a_fOuterRadius * cos(angle), centerTop.y, a_fOuterRadius * sin(angle));
	}

	// Create the points for the bottom and top of the inner ring of the tube
	vector3* bottomInnerPoints = new vector3[a_nSubdivisions];
	vector3* topInnerPoints = new vector3[a_nSubdivisions];
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// Find the angle that we are using to make the point
		float angle = 2 * glm::pi<float>() * (float)i / a_nSubdivisions;

		// Create the two points with the x values being the inner radius times the cos of the angle, and the z values being the inner radius times the sin of the angle
		bottomInnerPoints[i] = vector3(a_fInnerRadius * cos(angle), centerBottom.y, a_fInnerRadius * sin(angle));
		topInnerPoints[i] = vector3(a_fInnerRadius * cos(angle), centerTop.y, a_fInnerRadius * sin(angle));
	}

	// Create the quads for the side of the tube, using the Outer Points
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// Make a quad using the points at i and i + 1 in our arrays
		if (i != a_nSubdivisions - 1)
		{
			AddQuad(bottomOuterPoints[i + 1], bottomOuterPoints[i], topOuterPoints[i + 1], topOuterPoints[i]);
		}
		// We have a special case at our last index, because we are using the points at the start and end of our arrays instead
		else
		{
			AddQuad(bottomOuterPoints[0], bottomOuterPoints[i], topOuterPoints[0], topOuterPoints[i]);
		}
	}

	// Create the quads for the top and bottom of the tube
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// Make a quad using the points at i and i + 1 in our arrays
		if (i != a_nSubdivisions - 1)
		{
			AddQuad(topOuterPoints[i + 1], topOuterPoints[i], topInnerPoints[i + 1], topInnerPoints[i]);
			AddQuad(bottomOuterPoints[i], bottomOuterPoints[i + 1], bottomInnerPoints[i], bottomInnerPoints[i + 1]);
		}
		// We have a special case at our last index, because we are using the points at the start and end of our arrays instead
		else
		{
			AddQuad(topOuterPoints[0], topOuterPoints[i], topInnerPoints[0], topInnerPoints[i]);
			AddQuad(bottomOuterPoints[i], bottomOuterPoints[0], bottomInnerPoints[i], bottomInnerPoints[0]);
		}
	}

	// Create the quads for the inner sides of the tube, using the Inner Points
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// Make a quad using the points at i and i + 1 in our arrays
		if (i != a_nSubdivisions - 1)
		{
			AddQuad(bottomInnerPoints[i], bottomInnerPoints[i + 1], topInnerPoints[i], topInnerPoints[i + 1]);
		}
		// We have a special case at our last index, because we are using the points at the start and end of our arrays instead
		else
		{
			AddQuad(bottomInnerPoints[i], bottomInnerPoints[0], topInnerPoints[i], topInnerPoints[0]);
		}
	}

	// Delete the arrays of points as they are no longer needed
	delete[] topInnerPoints;
	delete[] topOuterPoints;
	delete[] bottomInnerPoints;
	delete[] bottomOuterPoints;

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	GenerateCube(a_fOuterRadius * 2.0f, a_v3Color);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	// Replace this with your code
	// Create the top and bottom center points
	float fHeight = a_fRadius / 2.0f;
	vector3 centerTop(0, fHeight, 0);
	vector3 centerBottom(0, -fHeight, 0);

	// Create a cylinder as the middle of the circle

	// Create the points for the bottom and top of the cylinder
	vector3* bottomPoints = new vector3[a_nSubdivisions];
	vector3* topPoints = new vector3[a_nSubdivisions];
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// Find the angle that we are using to make the point
		float angle = 2 * glm::pi<float>() * (float)i / a_nSubdivisions;

		// Create the two points with the x values being the radius times the cos of the angle, and the z values being the radius times the sin of the angle
		bottomPoints[i] = vector3(a_fRadius * cos(angle), centerBottom.y, a_fRadius * sin(angle));
		topPoints[i] = vector3(a_fRadius * cos(angle), centerTop.y, a_fRadius * sin(angle));
	}

	// Create the quads for the side of the cylinder
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		if (i != a_nSubdivisions - 1)
		{
			AddQuad(bottomPoints[i + 1], bottomPoints[i], topPoints[i + 1], topPoints[i]);
		}
		else
		{
			AddQuad(bottomPoints[0], bottomPoints[i], topPoints[0], topPoints[i]);
		}
	}


	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}