#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Kiernan Brown - kwb6721@g.rit.edu";

	//Alberto needed this at this position for software recording.
	m_pWindow->setPosition(sf::Vector2i(710, 0)); 
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	/* If there are no stops in our configuration file, we will make these default stops
	Stop: [-3.000, -3.000, -3.000]
	Stop : [-3.000, 3.000, 0.000]
	Stop : [-2.000, -2.000, -3.000]
	Stop : [-2.000, 2.000, 0.000]
	Stop : [-1.000, -1.000, -3.000]
	Stop : [-1.000, 1.000, 0.000]
	Stop : [0.000, 0.000, 0.000]
	Stop : [1.000, -1.000, 0.000]
	Stop : [1.000, 1.000, -3.000]
	Stop : [2.000, -2.000, 0.000]
	Stop : [2.000, 2.000, -3.000]
	Stop : [3.000, -3.000, 0.000]
	Stop : [3.000, 3.000, -3.000]*/
	if (m_stopsList.size() == 0)
	{
		m_stopsList.push_back(vector3(-3.0, -3.0, -3.0));
		m_stopsList.push_back(vector3(-3.0, 3.0, 0.0));
		m_stopsList.push_back(vector3(-2.0, -2.0, -3.0));
		m_stopsList.push_back(vector3(-2.0, 2.0, 0.0));
		m_stopsList.push_back(vector3(-1.0, -1.0, -3.0));
		m_stopsList.push_back(vector3(-1.0, 1.0, 0.0));
		m_stopsList.push_back(vector3(0.0, 0.0, 0.0));
		m_stopsList.push_back(vector3(1.0, -1.0, 0.0));
		m_stopsList.push_back(vector3(1.0, 1.0, -3.0));
		m_stopsList.push_back(vector3(2.0, -2.0, 0.0));
		m_stopsList.push_back(vector3(2.0, 2.0, -3.0));
		m_stopsList.push_back(vector3(3.0, -3.0, 0.0));
		m_stopsList.push_back(vector3(3.0, 3.0, -3.0));
	}
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//Get a timer
	static float fTimer = 0;							// Store the new timer
	static uint uClock = m_pSystem->GenClock();			// Generate a new clock for that timer
	float fDeltaTime = m_pSystem->GetDeltaTime(uClock); // Get the delta time for that timer
	fTimer += fDeltaTime;								// Add the delta time	

	static uint path = 0;	// uint that represents the path we are currently on

	vector3 startPoint = m_stopsList[path];								// startPoint is the starting point of the path we are on
	vector3 endPoint = m_stopsList[(path + 1) % m_stopsList.size()];	// endPoint is the next point we are moving to 

	float pathTime = 2.0f;													// Amount of time it takes to complete a path
	float pathPercentage = MapValue(fTimer, 0.0f, pathTime, 0.0f, 1.0f);	// Percentage of the path we are done with
																			// Calculate the current position
	vector3 v3CurrentPos = glm::lerp(startPoint, endPoint, pathPercentage);

	// When we finish this path, we reset the timer and go to the next path
	if (pathPercentage >= 1.0f)
	{
		path++;
		fTimer = m_pSystem->GetDeltaTime(uClock);
		path %= m_stopsList.size();
	}

	matrix4 m4Model = glm::translate(v3CurrentPos);

	// Rotate the torus so it face the screen
	static matrix4 m4Offset = glm::rotate(IDENTITY_M4, 90.0f, AXIS_X);
	
	static quaternion qStart;	// Quaternion for rotating the torus

	// If the torus is rotating, we will rotate it
	if (rotating)
	{
		quaternion qRotate = glm::angleAxis(fDeltaTime * 100.0f, vector3(1.0f, 0.0f, 0.0f));
		qStart = qStart * qRotate;
	}

	// Adjust the torus's matrix using our quaternion for rotation and our offset
	m4Model *= glm::toMat4(qStart) * m4Offset;

	m_pMeshMngr->AddTorusToRenderList(m4Model, C_PURPLE, RENDER_SOLID | RENDER_WIRE);

	m_pMeshMngr->Print("\nTimer: ");//Add a line on top
	m_pMeshMngr->PrintLine(std::to_string(fTimer), C_YELLOW);

	// Draw stops
	for (uint i = 0; i < m_stopsList.size(); ++i)
	{
		m_pMeshMngr->AddSphereToRenderList(glm::translate(m_stopsList[i]) * glm::scale(vector3(0.05f)), C_RED, RENDER_SOLID | RENDER_WIRE);
	}
	
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
	//release GUI
	ShutdownGUI();
}