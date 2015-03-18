#include "Entity.h"

Entity::Entity(float in_x, float in_y, float in_width, float in_height) {
	position.x = in_x;
	position.y = in_y;
	width = in_width;
	height = in_height;
}
Entity::~Entity() { }

bool Entity::Collide(Entity& ref_Other) {
	if (position.x > ref_Other.position.x + ref_Other.width || ref_Other.position.x > position.x + width) {
		return false; //no collision
	}
	if (position.y > ref_Other.position.y + ref_Other.height || ref_Other.position.y > position.y + height) {
		return false; //no collision
	}
	return true;
}

bool Entity::RayCast(Point in_Origin, Point in_direction) {
	//set ray O to end1
	//find magnitude of end to from origin of end1
	float ray_Magnitude = std::sqrt(std::pow(in_direction.x - in_Origin.x, 2) + std::pow(in_direction.y - in_Origin.y, 2));

	//set Direction of the ray
	float ray_R_x = (in_direction.x - in_Origin.x) / ray_Magnitude;
	float ray_R_y = (in_direction.y - in_Origin.y) / ray_Magnitude;

	//get minimum and maximum values for this box
	float min_x, min_y;
	min_x = position.x;
	min_y = position.y;
	float max_x, max_y;
	max_x = position.x + width;
	max_y = position.y + height;

	//setup ray min/max
	float tmin = (min_x - in_Origin.x) / ray_R_x;
	float tmax = (max_x - in_Origin.x) / ray_R_x;
	if (tmin > tmax) {
		float temp = tmin;
		tmin = tmax;
		tmax = temp;
	}

	float tymin = (min_y - in_Origin.y) / ray_R_y;
	float tymax = (max_y - in_Origin.y) / ray_R_y;
	if (tymin > tymax) {
		float temp = tymin;
		tymin = tymax;
		tymax = temp;
	}

	//if the ray misses
	if ((tmin > tymax) || (tymin > tmax)) {
		return false;
	}

	if (tymin > tmin) {
		tmin = tymin;
	}

	if (tymax < tmax) {
		tmax = tymax;
	}

	//check if the box we hit is within the line
	if ((tmin > ray_Magnitude) || (tmax < 0)) {
		return false;
	}
	return true;
}

float Entity::GetWidth() {
	return width;
}
float Entity::GetHeight() {
	return height;
}