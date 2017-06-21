#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Kiernan Brown - kwb6721@g.rit.edu";

	//Alberto needed this at this position for software recording.
	m_pWindow->setPosition(sf::Vector2i(710, 0)); 

	//init the mesh

	// We need to create 46 cubes in order to make the space invader shape
	int numCubes = 46;

	// Loop to create the cubes and add them to a vector called m_pMeshObjects
	for (int i = 0; i < numCubes; i++)
	{
		MyMesh* m_pMesh = new MyMesh();
		m_pMeshObjects.push_back(m_pMesh);
		m_pMeshObjects[i]->GenerateCube(1.0f, C_BLACK);
	}
	
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	static float value = 0.0f;
	
	// Render all the objects
	for (int i = 0; i < m_pMeshObjects.size(); i++)
	{
		matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(1.0f, 1.0f, 1.0f));
		matrix4 m4Translate;

		// Adjust the x and y values of the translate matrix in order to produce the space invader shape
		// We are also adjusting the x values by the value variable, and the y values by sin(value) in order to make the shape move over time
		if (i == 0)
		{
			m4Translate = glm::translate(IDENTITY_M4, vector3(-3.0f + value, 4.0f + sin(value), 0.0f));
		}
		if (i == 1)
		{
			m4Translate = glm::translate(IDENTITY_M4, vector3(3.0f + value, 4.0f + sin(value), 0.0f));
		}
		if (i == 2)
		{
			m4Translate = glm::translate(IDENTITY_M4, vector3(-2.0f + value, 3.0f + sin(value), 0.0f));
		}
		if (i == 3)
		{
			m4Translate = glm::translate(IDENTITY_M4, vector3(2.0f + value, 3.0f + sin(value), 0.0f));
		}
		if (i >= 4 && i < 11)
		{
			m4Translate = glm::translate(IDENTITY_M4, vector3(i - 7 + value, 2.0f + sin(value), 0.0f));
		}
		if (i >= 11 && i < 13)
		{
			m4Translate = glm::translate(IDENTITY_M4, vector3(i - 15 + value, 1.0f + sin(value), 0.0f));
		}
		if (i >= 13 && i < 16)
		{
			m4Translate = glm::translate(IDENTITY_M4, vector3(i - 14 + value, 1.0f + sin(value), 0.0f));
		}
		if (i >= 16 && i < 18)
		{
			m4Translate = glm::translate(IDENTITY_M4, vector3(i - 13 + value, 1.0f + sin(value), 0.0f));
		}
		if (i >= 18 && i < 29)
		{
			m4Translate = glm::translate(IDENTITY_M4, vector3(i - 23 + value, 0.0f + sin(value), 0.0f));
		}
		if (i == 29)
		{
			m4Translate = glm::translate(IDENTITY_M4, vector3(-5.0f + value, -1.0f + sin(value), 0.0f));
		}
		if (i >= 30 && i < 37)
		{
			m4Translate = glm::translate(IDENTITY_M4, vector3(i - 33 + value, -1.0f + sin(value), 0.0f));
		}
		if (i == 37)
		{
			m4Translate = glm::translate(IDENTITY_M4, vector3(5.0f + value, -1.0f + sin(value), 0.0f));
		}
		if (i == 38)
		{
			m4Translate = glm::translate(IDENTITY_M4, vector3(-5.0f + value, -2.0f + sin(value), 0.0f));
		}
		if (i == 39)
		{
			m4Translate = glm::translate(IDENTITY_M4, vector3(-3.0f + value, -2.0f + sin(value), 0.0f));
		}
		if (i == 40)
		{
			m4Translate = glm::translate(IDENTITY_M4, vector3(3.0f + value, -2.0f + sin(value), 0.0f));
		}
		if (i == 41)
		{
			m4Translate = glm::translate(IDENTITY_M4, vector3(5.0f + value, -2.0f + sin(value), 0.0f));
		}
		if (i >= 42 && i < 44)
		{
			m4Translate = glm::translate(IDENTITY_M4, vector3(i - 44 + value, -3.0f + sin(value), 0.0f));
		}
		if (i >= 44 && i < 46)
		{
			m4Translate = glm::translate(IDENTITY_M4, vector3(i - 43 + value, -3.0f + sin(value), 0.0f));
		}

		matrix4 m4Model = m4Scale * m4Translate;

		m_pMeshObjects[i]->Render(m4Projection, m4View, m4Model);
	}
	value += 0.01f;
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	// Loop through m_pMeshObjects and SafeDelete each object
	for (int i = 0; i < m_pMeshObjects.size(); i++)
	{
		SafeDelete(m_pMeshObjects[i]);
	}
	

	//release GUI
	ShutdownGUI();
}