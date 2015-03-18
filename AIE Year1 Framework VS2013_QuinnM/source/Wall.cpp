#include "Wall.h"

char* Wall::wallTexture = "images/crate_sideup.png";
unsigned int Wall::wallSprite = 0;

Wall::Wall(float in_posX, float in_posY, float in_width, float in_height) : Entity(in_posX, in_posY, in_width, in_height){
	if (wallSprite == 0) {
		wallSprite = CreateSprite(wallTexture, width, height, true);
	}
}
Wall::~Wall() { };

void Wall::Update() {
	//do nothing
}
void Wall::Draw() {
	MoveSprite(wallSprite, position.x, position.y);
	DrawSprite(wallSprite);
}

void Wall::MoveTo(float in_x, float in_y) {
	position.x = in_x;
	position.y = in_y;
}