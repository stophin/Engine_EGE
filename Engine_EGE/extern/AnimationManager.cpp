
#include "AnimationManager.h"

AnimationManager::AnimationManager():
uniqueId(0),
prev(NULL),
next(NULL)
{
}
AnimationManager::~AnimationManager()
{
}

Animations * AnimationManager::addAnimation(Animations * animation, INT uniqueID)
{
	animation->uniqueID = uniqueID;
	this->animation.insertLink(animation);

	return animation;
}

VOID AnimationManager::moveAnimation(FLOAT directionX, FLOAT directionY)
{
	if (this->animation.link == NULL)
	{
		return;
	}
	Animations * temp = this->animation.link;
	do
	{
		temp->moveSequence(directionX, directionY);

		temp = temp->next;
	} while (temp && temp != this->animation.link);
}

VOID AnimationManager::removeAnimation(Animations * animation)
{
	this->animation.removeLink(animation);
}

INT AnimationManager::drawAnimation(Graphics & graphics)
{
	if (this->animation.link == NULL)
	{
		return 0;
	}
	return this->animation.link->drawSequence(graphics);
}

VOID AnimationManager::nextAnimation(void * lParam, int moving)
{
	if (this->animation.link == NULL)
	{
		return;
	}
	this->animation.link->nextSequence(lParam, moving);
	//Animations * temp = this->animation.link;
	//do
	//{
	//	temp->nextSequence(lParam, moving);

			// Only the current animation can do trigger
			// When sequence is done
			// Simply set lParam to null
			// will prevent other animations do trigger
//			lParam = NULL;
	//	temp = temp->next;
//	} while (temp && temp != this->animation.link);
}