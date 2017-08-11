#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	m_pMapMngr = MapManager::GetInstance();

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 5.0f, 25.0f), //Position
		vector3(0.0f, 0.0f, 0.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	// Set up our Map
	m_pMap = new Map("Maps\\Map.obj", "Map");
	m_pMapMngr->AddMap(m_pMap);
	m_pMapMngr->SetCurrentMap(0);
	m_pSelectedNode = m_pMap->GetNode(vector2(1, 2));

	// Initialize our pathfinder
	m_pPathfinder = new Pathfinder(m_pMapMngr->GetCurrentMap());
	
	// Add Player
	m_pEntityMngr->AddEntity("Minecraft\\Steve.obj", "Player");
	m_pEntityMngr->GetEntity(m_pEntityMngr->GetEntityIndex("Player"))->SetCurrentNode(m_pMapMngr->GetCurrentMap()->GetNode(vector2(4, 5)));

	// Add Enemies
	m_pEntityMngr->AddEntity("Minecraft\\Zombie.obj", "Enemy_1");
	m_pEntityMngr->AddEntity("Minecraft\\Zombie.obj", "Enemy_2");
	m_pEntityMngr->AddEntity("Minecraft\\Zombie.obj", "Enemy_3");
	m_pEntityMngr->GetEntity(m_pEntityMngr->GetEntityIndex("Enemy_1"))->SetCurrentNode(m_pMapMngr->GetCurrentMap()->GetNode(vector2(8, 12)));
	m_pEntityMngr->GetEntity(m_pEntityMngr->GetEntityIndex("Enemy_2"))->SetCurrentNode(m_pMapMngr->GetCurrentMap()->GetNode(vector2(20, 5)));
	m_pEntityMngr->GetEntity(m_pEntityMngr->GetEntityIndex("Enemy_3"))->SetCurrentNode(m_pMapMngr->GetCurrentMap()->GetNode(vector2(16, 14)));
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Update Entity Manager
	m_pEntityMngr->Update();

	// Display Planes if we are using the Plane Overlay
	if (m_bPlaneOverlay)
	{
		for (int i = 0; i < m_pMap->m_pMapNodes.size(); i++)
		{
			// Walkable tiles are displayed in green, yellow, or orange based on their Terrain Count
			if (m_pMap->m_pMapNodes[i]->IsWalkable())
			{
				// We position the plane at a y value higher than the center to have the plane hover over the ground slightly
				vector3 planePos = m_pMap->m_pMapNodes[i]->m_v3Center;
				planePos.y += m_pMap->GetNodeSize().y * 0.25f;
				vector3 planeColor;
				float tCost = m_pMap->m_pMapNodes[i]->GetTerrainCost(); 
				if (tCost == 1.0f)
				{
					planeColor = vector3(0, 1, 0);
				}
				else if (tCost == 2.0f)
				{
					planeColor = vector3(1, 1, 0);
				}
				else if(tCost == 3.0f)
				{
					planeColor = vector3(1, 0.65f, 0);
				}
				m_pMeshMngr->AddPlaneToRenderList(glm::translate(planePos) * glm::rotate(IDENTITY_M4, -90.0f, vector3(1, 0, 0)) * glm::scale(vector3(1.2f)), vector3(planeColor), 1);
			}
			// Non walkable tiles are displayed in red
			else
			{
				vector3 planePos = m_pMap->m_pMapNodes[i]->m_v3Center;
				planePos.y += m_pMap->GetNodeSize().y * 0.25f;
				m_pMeshMngr->AddPlaneToRenderList(glm::translate(planePos) * glm::rotate(IDENTITY_M4, -90.0f, vector3(1, 0, 0)) * glm::scale(vector3(1.2f)), vector3(1, 0, 0), 1);
			}
		}
	}

	// Add map to render list
	m_pMap->AddToRenderList(false);

	// Draw the cone pointing to the currently selected node
	vector3 v3ConePos = m_pSelectedNode->GetCenter();
	v3ConePos.y += 1.0f;
	m_pMeshMngr->AddConeToRenderList(glm::translate(v3ConePos) * glm::rotate(IDENTITY_M4, 180.0f, vector3(1, 0, 0)) * glm::scale(vector3(0.75f)), vector3(1, 0, 1), RENDER_SOLID);

	// Add objects to render lsit
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui,
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//Release MyEntityManager
	MyEntityManager::ReleaseInstance();
	MapManager::ReleaseInstance();

	SafeDelete(m_pPathfinder);

	//release GUI
	ShutdownGUI();
}