#pragma once

#include "ObjectManager.h"
#include "LinkList.h"

extern class Roles;

class Animations
{
public:
	Animations();
	~Animations();

	INT delay;
	INT delayCount;

	Animation_Mode::type mode;

	LinkList<ObjectManager> objects;

	ObjectManager * addSequence(ObjectManager * objects, INT uniqueID);
	VOID removeSequence(ObjectManager * objects);

	INT drawSequence(Graphics& graphics);

	VOID nextSequence(void * lParam, int moving);

	VOID moveSequence(FLOAT directionX, FLOAT directionY);

	INT uniqueID;

	Animations * prev;
	Animations * next;
};

