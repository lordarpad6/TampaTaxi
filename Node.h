#pragma once
//Standard
#include "vector"

//User defined
#include"Object3d.h"
#include"Drawable.h"
#define NODE_DIAMETER 50. 
#define SCALLING 100000

class Node: public Object3d, public Drawable
{
	std::vector<long> ways;
public:
	Node(){}
	Node(Point center);
	void AddWay(long id);
	std::vector<long>& GetWays();
	void Draw() override;
	void Translate(double dx, double dy, double dz);
	~Node(void);
};
