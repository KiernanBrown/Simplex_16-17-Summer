#include "Map.h"

Simplex::Map::Map(String a_sMapName, String a_sUniqueID) : MyEntity(a_sMapName, a_sUniqueID)
{
	m_pSystem = SystemSingleton::GetInstance();
	m_sMapName = a_sUniqueID + ".txt";
	LoadMap();
	SetNeighbors();
}

Simplex::Map::Map(Map const & other) : MyEntity(other)
{
	m_pSystem = other.m_pSystem;
	m_sMapName = other.m_sMapName;
	LoadMap();
	SetNeighbors();
}

Simplex::Map & Simplex::Map::operator=(Map const & other)
{
	if (this != &other)
	{
		m_pSystem = other.m_pSystem;
		m_sMapName = other.m_sMapName;
		LoadMap();
		SetNeighbors();
	}
	return *this;
}

Simplex::Map::~Map()
{
	Release();
}

void Simplex::Map::Release()
{
	for (int i = 0; i < m_pMapNodes.size(); i++)
	{
		SafeDelete(m_pMapNodes[i]);
	}
	m_pMapNodes.clear();
}

void Simplex::Map::LoadMap()
{
	String sRoot = m_pSystem->m_pFolder->GetFolderRoot();
	String sMapFile = sRoot + "Data\\Maps\\"+ m_sMapName;
	FileReader reader;

	// If the file doesn't exist, exit this method
	if (reader.ReadFile(sMapFile.c_str()) == OUT_ERR_FILE_MISSING)
	{
		return;
	}

	reader.Rewind();
	while (reader.ReadNextLine() == OUT_RUNNING)
	{
		String sWord = reader.GetFirstWord();

		int nLenght = reader.m_sLine.length();
		char* zsTemp = new char[nLenght];

		// Read in the Width of the map (in terms of MapNodes)
		if (sWord == "Width:")
		{
			int value;
			sscanf_s(reader.m_sLine.c_str(), "Width: %d", &value);
			m_width = value;
		}

		// Read in the Width of the map (in terms of MapNodes)
		else if (sWord == "Length:")
		{
			int value;
			sscanf_s(reader.m_sLine.c_str(), "Length: %d", &value);
			m_length = value;
		}

		// Read in the Width of the map (in terms of MapNodes)
		else if (sWord == "Height:")
		{
			int value;
			sscanf_s(reader.m_sLine.c_str(), "Height: %d", &value);
			m_height = value;
		}

		else if (sWord == "Nodes:")
		{
			// Set the size of each MapNode on the map
			vector3 size = GetRigidBody()->GetMaxGlobal() - GetRigidBody()->GetMinGlobal();
			m_v3NodeSize.x = size.x / m_width;
			m_v3NodeSize.y = size.y / m_height;
			m_v3NodeSize.z = size.z / m_length;

			// Loop through all nodes on the map and set their center, height, and walkability
			for (int x = 0; x < m_width; x++)
			{
				for (int z = 0; z < m_length; z++)
				{
					// If we don't have a node in the file, then create a default node
					if (reader.ReadNextLine() == OUT_DONE)
					{
						MapNode* thisNode = new MapNode(vector2(x, z), 1, true);
						thisNode->m_v3Center = vector3(((x + 1) * m_v3NodeSize.x) - (size.x / 2.0f) + m_v3NodeSize.x / 2.0f, 0, (z * m_v3NodeSize.z + m_v3NodeSize.z / 2.0f) - (size.z / 2.0f));
						m_pMapNodes.push_back(thisNode);
					}
					// Load a node if we have it in the file
					else
					{
						int value1;
						int value2;
						sscanf_s(reader.m_sLine.c_str(), "H: %d W: %d", &value1, &value2);
						MapNode* thisNode = new MapNode(vector2(x, z), value1, value2);
						thisNode->m_v3Center = vector3(((x + 1) * m_v3NodeSize.x) - (size.x / 2.0f) + m_v3NodeSize.x / 2.0f, (value1 - 1) * m_v3NodeSize.y, (z * m_v3NodeSize.z + m_v3NodeSize.z / 2.0f) - (size.z / 2.0f));
						m_pMapNodes.push_back(thisNode);
					}
				}
			}
		}

		delete[] zsTemp;
		zsTemp = nullptr;
	}
	reader.CloseFile();
}

Simplex::vector3 Simplex::Map::GetNodeSize()
{
	return m_v3NodeSize;
}

Simplex::MapNode* Simplex::Map::GetNode(vector2 a_v2Index)
{
	// Return the MapNode at the specified index
	for (int i = 0; i < m_pMapNodes.size(); i++)
	{
		if (m_pMapNodes[i]->GetIndex() == a_v2Index)
		{
			return m_pMapNodes[i];
		}
	}
	return nullptr;
}

void Simplex::Map::SetNeighbors()
{
	// Loop through all MapNodes in the map
	for (int x = 0; x < m_width; x++)
	{
		for (int z = 0; z < m_length; z++)
		{
			// Add the four neighboring nodes looking to the left, right, up, and down
			MapNode* currentNode = GetNode(vector2(x, z));
			MapNode* neighbor1 = GetNode(vector2(x - 1, z));
			MapNode* neighbor2 = GetNode(vector2(x + 1, z));
			MapNode* neighbor3 = GetNode(vector2(x, z - 1));
			MapNode* neighbor4 = GetNode(vector2(x, z + 1));
			if (neighbor1 != nullptr)
			{
				currentNode->AddNeighbor(neighbor1);
			}
			if (neighbor2 != nullptr)
			{
				currentNode->AddNeighbor(neighbor2);
			}
			if (neighbor3 != nullptr)
			{
				currentNode->AddNeighbor(neighbor3);
			}
			if (neighbor4 != nullptr)
			{
				currentNode->AddNeighbor(neighbor4);
			}
		}
	}
}
