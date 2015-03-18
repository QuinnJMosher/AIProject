#ifndef _Entity_h_
#define _Entity_h_

#include "Point.h"
#include <algorithm>

class Entity {//abstract
public:
	Entity(float in_x, float in_y, float in_width, float in_height);
	~Entity();

	virtual void Update() = 0;
	virtual void Draw() = 0;

	bool Collide(Entity& ref_Other);
	bool RayCast(Point in_Origin, Point in_direction);

	Point position;

	float GetWidth();
	float GetHeight();

protected:
	float width;
	float height;
};

#endif