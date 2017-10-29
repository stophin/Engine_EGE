#pragma once

#include "Animations.h"
#include "LinkList.h"

class AnimationManager
{
public:
	AnimationManager();
	~AnimationManager();

	LinkList<Animations> animation;

	Animations * addAnimation(Animations * animation, INT uniqueID);
	VOID removeAnimation(Animations * animation);

	INT drawAnimation(Graphics & graphics);
	VOID nextAnimation(void * lParam, int moving);
	VOID moveAnimation(FLOAT directionX, FLOAT directionY);

	INT uniqueId;

	AnimationManager * prev;
	AnimationManager * next;
};

