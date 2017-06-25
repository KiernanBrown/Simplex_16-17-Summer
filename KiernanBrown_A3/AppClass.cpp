#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Kiernan Brown - kwb6721@g.rit.edu";

	//Alberto needed this at this position for software recording.
	m_pWindow->setPosition(sf::Vector2i(710, 0));
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndView(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
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

	// Create a timer
	static float fTimer = 0;
	static uint uClock = m_pSystem->GenClock();

	// Get the delta time
	fTimer += m_pSystem->GetDeltaTime(uClock);

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, 90.0f, AXIS_Z);

	float pathTime = 0.5f;
	static bool bInit = false;

	// If we haven't initialized the paths, we will do so now
	if (!bInit)
	{
		// fHeight is ued to keep track of the height of the torus
		float fHeight = 1.0f;

		// Loop to create the vectors of points on the paths
		for (uint i = 0; i < m_uOrbits; i++)
		{
			// Number of sides of the torus is always i + 3
			int numSides = i + 3;
			
			// Add a uint path for this torus to our paths vector
			static uint path = 0;
			paths.push_back(path);

			// Create a vector used to hold the points in the path for this torus
			std::vector<vector3> points;

			// Old hard coded values, left in to show my original though process
			/*if (i == 0)
			{
				points.push_back(vector3(fHeight, 0.0f, 0.0f));				// 1
				points.push_back(vector3(-fHeight / 2.0f, 0.8f, 0.0f));		// 2
				points.push_back(vector3(-fHeight / 2.0f, -0.8f, 0.0f));	// Reverse y of 2
				
			}
			if (i == 1)
			{
				points.push_back(vector3(fHeight, 0.0f, 0.0f));		// 1
				points.push_back(vector3(0.0f, fHeight, 0.0f));		// 2
				points.push_back(vector3(-fHeight, 0.0f, 0.0f));	// Reverse x of 1
				points.push_back(vector3(0.0f, -fHeight, 0.0f));	// Reverse y of 2
			}
			if (i == 2)
			{
				// fHeight = 2
				// Sides = 5
				points.push_back(vector3(fHeight, 0.0f, 0.0f));	// 1
				points.push_back(vector3(0.6f, 1.9f, 0.0f));	// 2
				points.push_back(vector3(-1.6f, 1.1f, 0.0f));	// 3
				points.push_back(vector3(-1.6f, -1.1f, 0.0f));	// Reverse y of 3
				points.push_back(vector3(0.6f, -1.9f, 0.0f));	// Reverse y of 2
			}
			if (i == 3)
			{
				points.push_back(vector3(fHeight, 0.0f, 0.0f));						// 1
				points.push_back(vector3(fHeight / 2.0f, fHeight * 0.85f, 0.0f));	// 2
				points.push_back(vector3(-fHeight / 2.0f, fHeight * 0.85f, 0.0f));	// Reverse x of 2
				points.push_back(vector3(-fHeight, 0.0f, 0.0f));					// Reverse x of 1
				points.push_back(vector3(-fHeight / 2.0f, -fHeight * 0.85f, 0.0f));	// Reverse x and y of 2
				points.push_back(vector3(fHeight / 2.0f, -fHeight * 0.85f, 0.0f));	// Reverse y of 2
			}
			if (i == 4)
			{
				// fHeight = 3
				// Sides = 7
				points.push_back(vector3(fHeight, 0.0f, 0.0f));		// 1
				points.push_back(vector3(1.9f, 2.3f, 0.0f));		// 2
				points.push_back(vector3(-0.7f, 2.9f, 0.0f));		// 3
				points.push_back(vector3(-2.65f, 1.25f, 0.0f));		// 4
				points.push_back(vector3(-2.65f, -1.25f, 0.0f));	// Reverse y of 4
				points.push_back(vector3(-0.7f, -2.9f, 0.0f));		// Reverse y of 3
				points.push_back(vector3(1.9f, -2.3f, 0.0f));		// Reverse y of 2
			}
			if (i == 5)
			{
				points.push_back(vector3(fHeight, 0.0f, 0.0f));		// 1
				points.push_back(vector3(2.45f, 2.45f, 0.0f));		// 2
				points.push_back(vector3(0.0f, fHeight, 0.0f));		// 3
				points.push_back(vector3(-2.45f, 2.45f, 0.0f));		// Reverse x of 2
				points.push_back(vector3(-fHeight, 0.0f, 0.0f));	// Reverse x of 1
				points.push_back(vector3(-2.45f, -2.45f, 0.0f));	// Reverse x and y of 2
				points.push_back(vector3(0.0f, -fHeight, 0.0f));	// Reverse y of 3
				points.push_back(vector3(2.45f, -2.45f, 0.0f));		// Reverse y of 2
			}
			if (i == 6)
			{
				// fHeight = 4
				// Sides = 9
				/*points.push_back(vector3(fHeight, 0.0f, 0.0f));		// 1
				points.push_back(vector3(3.0f, 2.65f, 0.0f));			// 2
				points.push_back(vector3(0.6f, 3.9f, 0.0f));			// 3
				points.push_back(vector3(-2.0f, 3.4f, 0.0f));			// 4
				points.push_back(vector3(-3.7f, 1.3f, 0.0f));			// 5
				points.push_back(vector3(-3.7f, -1.3f, 0.0f));			// Reverse y of 5
				points.push_back(vector3(-2.0f, -3.4f, 0.0f));			// Reverse y of 4
				points.push_back(vector3(0.6f, -3.9f, 0.0f));			// Reverse y of 3
				points.push_back(vector3(3.0f, -2.65f, 0.0f));			// Reverse y of 2
			}
			// First value is always (fHeight, 0, 0)
			// This could be replicated with (cos(0) * fHeight, sin(0) * fHeight, 0)
			// Using sin and cos would also give that reversing the x or y values of a previous point that is seen above
			*/

			// Loop for the number of sides of this torus to create the points on the path
			for (int j = 0; j < numSides; j++)
			{
				// Get the angle from the center that the current point is at
				float angle = 2 * glm::pi<float>() * (float)j / numSides;

				// The x value will be the fHeight of the torus * the cos of the angle
				// The y value will be the fHeight of the torus * the sin of the angle
				// We subtract 0.05f from the fHeight to center the spheres on the points, since 0.05f is half of the sphere's radius
				points.push_back(vector3((fHeight - 0.05f) * cos(angle), (fHeight - 0.05f) * sin(angle), 0.0f));
			}

			// If we are rotating, we add the start point again to keep the sphere in position when rotating the torus
			if (rotating)
			{
				points.push_back(vector3(fHeight - 0.05f, 0.0f, 0.0f));
			}

			// Add the vector that we have created to our vector pathPoints vector
			pathPoints.push_back(points);

			// Set the rotation quaternion and float value of the torus
			quaternion rotQuaternion;
			rotations.push_back(rotQuaternion);
			rotFloats.push_back(0.0f);

			// Increment fHeight by 0.05f
			fHeight += 0.5f;
		}

		// We have initialized all of the paths
		bInit = true;
	}

	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset * glm::toMat4(rotations[i]), 90.0f, AXIS_X));

		vector3 startPoint = pathPoints[i][paths[i]];								// startPoint is the starting point of the path we are on
		vector3 endPoint = pathPoints[i][(paths[i] + 1) % pathPoints[i].size()];	// endPoint is the next point we are moving to 

		float pathPercentage = MapValue(fTimer, 0.0f, pathTime, 0.0f, 1.0f);	// Percentage of the path we are done with
		
		// Calculate the current position
		vector3 v3CurrentPos = glm::lerp(startPoint, endPoint, pathPercentage);

		// When we finish this path, we reset the timer and go to the next path for all of the spheres
		if (pathPercentage >= 1.0f)
		{
			// Reset the timer
			fTimer = m_pSystem->GetDeltaTime(uClock);

			// Loop through our paths vector and change the path for every sphere, as they will always reach their next points at the same time
			for (int j = 0; j < m_uOrbits; j++)
			{
				paths[j]++;
				paths[j] %= pathPoints[j].size();

				// If we have just rotated the torus, increase the float value for that torus
				if (paths[j] == 0)
				{
					rotFloats[j] += 90.0f;
				}
			}
		}

		// If we are rotating the tori, we will rotate a torus 90 degrees when a sphere has completed its orbit
		if (rotating)
		{
			for (int j = 0; j < m_uOrbits; j++)
			{
				// If we have completed the entire path and we are rotating, we will rotate the torus now using SLERP
				if (paths[j] == pathPoints[j].size() - 1)
				{
					// q1 is our current quaternion
					quaternion q1 = rotations[j];

					// q2 is the quaternion we are going to
					quaternion q2 = glm::angleAxis((rotFloats[j] + 90.0f), vector3(0.0f, 0.0f, 1.0f));

					// Use SLERP and change our current quaternion to be the qSLERP
					float fPercentage = MapValue(fTimer, 0.0f, 13.0f, 0.0f, 1.0f);
					quaternion qSLERP = glm::mix(q1, q2, fPercentage);
					rotations[j] = qSLERP;
				}
			}
		}

		matrix4 m4Model = glm::translate(m4Offset * glm::toMat4(rotations[i]), v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

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