#include "Texture.h"
#include "Map.h"
#include "Tools.h"

#include "cmath"
#include "glut.h"

#define SIZE_OF_GROUND 10000

Map::Map(char *nodesFile, char *buildingsFile)
:Object3d(Point()), topLeftMapPoint(Point(+1000000, 0, +1000000)), bottomRightPoint(Point(-1000000, 0, -100000))
{
	currentWayName = NULL;
	loadNodes(nodesFile);
	loadBuildings(buildingsFile);
	initQuadTree();
	initMinimap();
}
void Map::loadNodes(char *nodesFile)
{
	Tools::ReadNodesFromXML(nodesFile, nodes, ways);
	center = nodes[START_MODAROM]->GetCenter();
	for(std::map<long, Node*>::iterator nodesIt = nodes.begin(); nodesIt != nodes.end(); ++nodesIt)
	{
		((*nodesIt).second)->Translate(-center.x, 0, -center.z);
		Point p = ((*nodesIt).second)->GetCenter();
		if (p.x < topLeftMapPoint.x)
			topLeftMapPoint.x = p.x;
		if (p.x > bottomRightPoint.x)
			bottomRightPoint.x = p.x;
		if (p.z < topLeftMapPoint.z)
			topLeftMapPoint.z = p.z;
		if (p.z > bottomRightPoint.z)
			bottomRightPoint.z = p.z;
	}
	
	for(std::map<long, Way*>::iterator waysIt = this->ways.begin(); waysIt != this->ways.end(); ++waysIt)
	{
		(*waysIt).second->ComputeStreet();
	}	
	waysToDraw.clear();
}
void Map::loadBuildings(char *buildingsFile)
{
	Tools::ReadBuildingsFromXML(buildingsFile, buildings);
	for(std::vector<Building*>::iterator buildingIt = buildings.begin(); buildingIt != buildings.end(); ++buildingIt)
	{
		(*buildingIt)->Translate(-center.x, 0, -center.z);
		Point p = (*buildingIt)->GetCenter();
		if (p.x < topLeftMapPoint.x)
			topLeftMapPoint.x = p.x;
		if (p.x > bottomRightPoint.x)
			bottomRightPoint.x = p.x;
		if (p.z < topLeftMapPoint.z)
			topLeftMapPoint.z = p.z;
		if (p.z > bottomRightPoint.z)
			bottomRightPoint.z = p.z;
	}
	buildingsToDraw.clear();
}
void Map::initQuadTree()
{
	drawableQuadTree = new QuadTree(0, Rectangl(topLeftMapPoint.x - 1, topLeftMapPoint.z - 1, bottomRightPoint.x - topLeftMapPoint.x + 1, bottomRightPoint.z - topLeftMapPoint.z + 1));
	for (std::map<long, Node*>::iterator nodesIt = nodes.begin(); nodesIt != nodes.end(); ++nodesIt)
	{
		drawableQuadTree->Insert(((*nodesIt).second));
	}
	for (std::vector<Building*>::iterator buildingIt = buildings.begin(); buildingIt != buildings.end(); ++buildingIt)
	{
		drawableQuadTree->Insert(*buildingIt);
	}
}
void Map::initMinimap()
{
	Point minimapCenter;

	minimapCenter.x = (topLeftMapPoint.x + bottomRightPoint.x) / 2;
	minimapCenter.y = 0;
	minimapCenter.z = (topLeftMapPoint.z + bottomRightPoint.z) / 2;

	miniMap = new Minimap(ways, minimapCenter);
}
void Map::drawGround()
{
	//Ground
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glScalef(SIZE_OF_GROUND,1,SIZE_OF_GROUND);
		glMatrixMode(GL_TEXTURE);
		glPushMatrix();
		glScaled(SIZE_OF_GROUND/2,SIZE_OF_GROUND/2,1);
		glBindTexture(GL_TEXTURE_2D,Texture::GetInstance().skyCube[5]);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f); glVertex3f( -1.0f, -0.10f, -1.0f);
				glTexCoord2f(0.0f, 1.0f); glVertex3f( -1.0f, -0.1f, +1.0f);
				glTexCoord2f(1.0f, 1.0f); glVertex3f( +1.0f, -0.10f, +1.0f);
				glTexCoord2f(1.0f, 0.0f); glVertex3f( +1.0f, -0.10f, -1.0f);
			glEnd();
		glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}
void Map::Draw()
{
	glEnable(GL_BLEND);
	drawGround();
	Point first, second;

	for(std::set<long>::iterator waysIt = waysToDraw.begin(); waysIt != waysToDraw.end(); ++waysIt)
		ways[*waysIt]->Draw();
	for(std::set<Building*>::iterator buildingIt = buildingsToDraw.begin(); buildingIt != buildingsToDraw.end(); ++buildingIt)
	{
		(*buildingIt)->Draw();
	}
	
	Point *checkPoint = miniMap->GetChekcpoint();
	if (checkPoint)
	{
		

		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1, 1, 0, 0.5);
		
		glPushMatrix();
		glTranslated(checkPoint->x, checkPoint->y, checkPoint->z);
		glRotatef(-90, 1,0,0);

		GLUquadricObj* quadric = gluNewQuadric();
		gluQuadricNormals(quadric, GLU_SMOOTH);
		gluQuadricTexture(quadric, GL_TRUE); 
		gluQuadricDrawStyle(quadric, GLU_FILL);
		gluCylinder(quadric, NODE_DIAMETER, NODE_DIAMETER, 100, 20, 1);

		glPopMatrix();
		glDisable(GL_BLEND);
	}

}
void Map::Update(Point camPosition, double camAngle)
{
	std::set<int> visitedQuadrants;
	std::set<Node*> nodes;

	currentPosition = camPosition;
	miniMap->UpdateCurrentPosition(camPosition);

	waysToDraw.clear();
	buildingsToDraw.clear();
	
	drawableQuadTree->Retrieve(buildingsToDraw, camPosition);
	drawableQuadTree->Retrieve(nodes, camPosition);
	
	int camPositionIndex = drawableQuadTree->GetNodeIndex(camPosition);
	visitedQuadrants.insert(camPositionIndex);
	double angle = camAngle - 90;
	for (double radius = 50; radius < 500; radius += 150)
	{
		for (int i = 0; i < 5; i++, angle += 45)
		{
			Point p = Point(camPosition.x + radius * cos(angle * PIdiv180), camPosition.y, camPosition.z + radius * sin(angle * PIdiv180));
			int nextPositionIndex = drawableQuadTree->GetNodeIndex(p);
			if (!visitedQuadrants.count(nextPositionIndex))
			{				
				drawableQuadTree->Retrieve(buildingsToDraw, p);
				if (radius < 201)
					drawableQuadTree->Retrieve(nodes, p);
				visitedQuadrants.insert(nextPositionIndex);
			}
		}
	}

	for(std::set<Node*>::iterator nodesIt = nodes.begin(); nodesIt != nodes.end(); ++nodesIt)
	{
		waysToDraw.insert((*nodesIt)->GetWays().begin(), (*nodesIt)->GetWays().end());
	}
}
Node* Map::GetNode(long id)
{
	return nodes[id];
}
Way* Map::GetWay(long id)
{
	return ways[id];
}
std::set<long>* Map::GetWaysToDraw()
{
	return &waysToDraw;
}
Minimap* Map::GetMinimap()
{
	return miniMap;
}

void Map::GenerateCheckpoint(double distance, Point &carCheckpoint, Point &humanCheckpoint)
{
	long random = 0;
	long nodeId, nodePos;
	do{
		random = rand();
		nodePos = random % nodes.size();
		std::map<long, Node*>::iterator it = nodes.begin();
		std::advance(it, nodePos);
		nodeId = (*it).first;
	} while (SF3dVector(currentPosition, nodes[nodeId]->GetCenter()).GetMagnitude() > distance);
	
	
	std::vector<long> nodeWays = nodes[nodeId]->GetWays();
	long wayId = nodeWays[random % nodeWays.size()];

	long portionIndex = random % (ways[wayId]->GetNodes().size() - 1);

	Street* street = ways[wayId]->GetRightSidewalk(portionIndex);
	SF3dVector v1, v2, vr;

	v1 = SF3dVector(street->corners[1], street->corners[0]);
	v2 = SF3dVector(street->corners[1], street->corners[2]);
	v1 = v1*0.50;
	v2 = v2*(((random % 41) + 30.0) / 100.0);
	vr = v1 + v2;
	vr.x += street->corners[1].x;
	vr.y += street->corners[1].y;
	vr.z += street->corners[1].z;
	
	humanCheckpoint.x = vr.x;
	humanCheckpoint.y = vr.y;
	humanCheckpoint.z = vr.z;

	
	street = ways[wayId]->GetPortionStreet(portionIndex);

	v1 = SF3dVector(street->corners[1], street->corners[0]);
	v2 = SF3dVector(street->corners[1], street->corners[2]);
	v1 = v1*0.50;
	v2 = v2*(((random % 41) + 30.0) / 100.0);
	vr = v1 + v2;
	vr.x += street->corners[1].x;
	vr.y += street->corners[1].y;
	vr.z += street->corners[1].z;

	carCheckpoint.x = vr.x;
	carCheckpoint.y = vr.y;
	carCheckpoint.z = vr.z;

	checkPoint.x = vr.x;
	checkPoint.y = vr.y;
	checkPoint.z = vr.z;
	miniMap->UpdateCheckpoint(&checkPoint);
	
}
char* Map::GetCurrentWayName()
{
	return currentWayName;
}

void Map::StreetCollision(Node *node, Point M, int &insidePoints)
{
	if (node == NULL)
		return;
	Point nodeCenter = node->GetCenter();
	std::vector<long> adjacentWays = node->GetWays();
	if (Tools::PointInsideCircle(M, nodeCenter, NODE_DIAMETER / 2))
	{
		insidePoints++;
		for (int adjW = 0; adjW < adjacentWays.size(); adjW++)
		{
			int index = ways[adjacentWays[adjW]]->GetIndex(node);
			int size = ways[adjacentWays[adjW]]->GetNodes().size();
			if ((index == 0 || index == size - 1) && insidePoints == 0)
			{
				Tools::UpdateIntersections(node->GetId());
			}
		}
		return;
	}
	
	for (int adjW = 0; adjW < adjacentWays.size(); adjW++)
	{
		Way* adjacentWay = ways[adjacentWays[adjW]];
		int nodeWayIndex = adjacentWay->GetIndex(node);
		Street *portionStreet = adjacentWay->GetPortionStreet(nodeWayIndex);
		if (portionStreet != NULL && Tools::PointInsideRectangle(M, portionStreet->corners[0], portionStreet->corners[1], portionStreet->corners[2], portionStreet->corners[3]))
		{
			if (insidePoints == 0)
			{
				if (currentWayName == NULL)
				{
					currentWayName = new char[70];
				}
				strcpy(currentWayName, adjacentWay->GetName());
			}
			insidePoints++;
			return;
		}
		portionStreet = adjacentWay->GetPortionStreet(nodeWayIndex - 1);
		if (portionStreet != NULL && Tools::PointInsideRectangle(M, portionStreet->corners[0], portionStreet->corners[1], portionStreet->corners[2], portionStreet->corners[3]))
		{
			if (insidePoints == 0)
			{
				if (currentWayName == NULL)
				{
					currentWayName = new char[70];
				}
				strcpy(currentWayName, adjacentWay->GetName());
			}
			insidePoints++;
			return;
		}
	}
}

Map::~Map(void)
{
}
