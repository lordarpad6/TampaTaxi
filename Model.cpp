#include "Model.h"

#define SPEED 0.3

Model::Model(void)
{
	brasovMap = new Map("StreetsRefactor.osm", "BuildingsRefactor.osm");
	worldGenerator = new WorldGenerator(brasovMap);
	worldGenerator->Initialize();

	car= new Car(Point());
	car->colliders=worldGenerator->GetVisibleCars();

	player = new Player(car);
	player->LastVisitedNodeIndex = START_NODE;
}

void Model::Update()
{
	sceneObjects.clear();
	sceneObjects.push_back(&skyCube);
	sceneObjects.push_back(car);
	sceneObjects.push_back(brasovMap);
	sceneObjects.push_back(worldGenerator);
	
	camera.SetPosition(player->GetPosition());

	//car->Update();
	skyCube.SetPoz(camera.GetPosition());
	brasovMap->Update(camera.GetPosition(), camera.GetRotY());

	worldGenerator->Update(camera.GetPosition());
	worldGenerator->HumanCallTaxi(player);
	for(std::vector<Collidable*>::iterator it=worldGenerator->GetVisibleHumans()->begin();it<worldGenerator->GetVisibleHumans()->end();++it)
		((Human*)(*it))->Update();
}
void Model::MoveUp()
{
	//car->Accelerate();
	car->SetAngle(camera.GetRotY());
	if(car->MoveWith(-5))
	{
		if(!playerMapCollision())
			car->MoveWith(5);
	}
	
		
}
void Model::MoveDown()
{
	//car->Reverse();
	car->SetAngle(camera.GetRotY());
	if(car->MoveWith(5))
	{
		if(!playerMapCollision())
			car->MoveWith(-5);
	}
}
void Model::MoveLeft()
{
	car->TurnLeft();
}
void Model::MoveRight()
{
	car->TurnRight();
}
void Model::MouseMove(double dx,double dy)
{
	double rotY=dx*0.12;
	double rotX=dy*0.12;
	camera.RotateY(rotY);
	camera.RotateX(rotX);
}

int Model::playerMapCollision()
{
	int insidePoints = 0;
	Point M;

	Collidable* collidable = player->GetPlayerState();
	if(collidable == NULL)
		return 0;
	
	for(int i = 0; i < 4; i++)
	{
		if(i == 0)
			M = collidable->GetTopRight();
		if(i == 1)
			M = collidable->GetBottomRight();
		if(i == 2)
			M = collidable->GetBottomLeft();
		if(i == 3)
			M = collidable->GetTopLeft();
		
		Node* lastVisitedNode = brasovMap->GetNode(player->LastVisitedNodeIndex);
		brasovMap->StreetCollision(lastVisitedNode, M, insidePoints);
		if (insidePoints != i + 1)
		{
			std::vector<long> adjacentWays = lastVisitedNode->GetWays();
			for(int adjW = 0; adjW < adjacentWays.size() && insidePoints != i + 1; adjW++)
			{
				Way* adjacentWay = brasovMap->GetWay(adjacentWays[adjW]);
				int nodeWayIndex = adjacentWay->GetIndex(lastVisitedNode);
				
				Node *node = adjacentWay->GetNode(nodeWayIndex - 1);
				if (node != NULL)
					brasovMap->StreetCollision(node, M, insidePoints);
				
				if(insidePoints != i + 1)
				{
					node = adjacentWay->GetNode(nodeWayIndex + 1);
					if(node != NULL)
						brasovMap->StreetCollision(node, M, insidePoints);
					if (insidePoints == i + 1)
						player->LastVisitedNodeIndex = node->GetId();
				}
				else
					player->LastVisitedNodeIndex = node->GetId();
			}
		}
		if (insidePoints != i + 1)
			break;
	}
	if(insidePoints == 4)
		return 1;
	return 0;
}

std::vector<Drawable*>* Model::GetSceneObjects()
{
	return &sceneObjects;
}

CCamera Model::GetCamera()
{
	return camera;
}

Player* Model::GetPlayer()
{
	return player;
}

Map* Model::GetMap()
{
	return brasovMap;
}

Model::~Model(void)
{
	delete brasovMap;
}
