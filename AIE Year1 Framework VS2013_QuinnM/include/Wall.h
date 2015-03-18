#ifndef _Wall_h_
#define _Wall_h_

#include "Entity.h"
#include "AIE.h"

class Wall : public Entity {
public:
	Wall(float in_posX, float in_posY, float in_width, float in_height);
	~Wall();

	virtual void Update();
	virtual void Draw();

	void MoveTo(float in_x, float in_y);

private:
	static char* wallTexture;
	static unsigned int wallSprite;
};

#endif