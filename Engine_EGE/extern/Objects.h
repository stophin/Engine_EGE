#pragma once

#include "Resources.h"


class World;

class Objects
{
public:
	Objects(Resources * resource, RectF& destination, RectF& truncation, FLOAT scale = 1);
	Objects();
	~Objects();

	Resources * resource;

	RectF destination;
	RectF truncation;

	World * world;

	INT reverse;

	INT drawObject(Graphics & graphics);

	VOID moveObject(FLOAT directionX, FLOAT directionY);

	INT uniqueID;

	Objects * prev;
	Objects * next;
};

