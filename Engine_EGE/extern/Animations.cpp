
#include "Animations.h"

Animations::Animations():
delay(0),
delayCount(0),
mode(Animation_Mode::Auto)
{
}

Animations::~Animations()
{
}

ObjectManager * Animations::addSequence(ObjectManager * objects, INT uniqueID)
{
	objects->uniqueID = uniqueID;
	this->objects.insertLink(objects);
	return objects;
}

VOID Animations::removeSequence(ObjectManager * objects)
{
	this->objects.removeLink(objects);
}

INT Animations::drawSequence(Graphics& graphics)
{
	if (this->objects.link == NULL)
	{
		return 0;
	}
	INT in = 0;
	ObjectManager * temp = this->objects.link->prev;
	do
	{
		in += temp->drawStep(graphics);
		temp = temp->prev;
	} while (temp && temp != this->objects.link->prev);
	return in;
}

VOID Animations::nextSequence(void * lParam, int moving)
{
	if (this->objects.link == NULL)
	{
		return;
	}
	if (this->mode == Animation_Mode::Still)
	{
		return;
	}
	if (this->delayCount++ >= this->delay)
	{
		this->delayCount = 0;
		ObjectManager * temp = this->objects.link;
		do
		{
			temp->object.link = temp->object.link->next;
			// when lParam is not null and mode is Step
			// do trigger when sequence is done
			if (lParam)
			{
				// Do not do trigger when moving if mode is Step
				// even if sequence is done
				if (lParam && moving)
				{
					temp->objectCount = -1;
				}
				if (this->mode == Animation_Mode::Step)
				{
					if (temp->objectCount > 0)
					{
						temp->objectCount = temp->object.linkcount;
					}
					temp->triggerFunction(lParam);
				}
				else if (this->mode == Animation_Mode::Auto)
				{

					temp->triggerFunction(lParam);
				}
			}

			temp = temp->next;
		} while (temp && temp != this->objects.link);
	}
}

VOID Animations::moveSequence(FLOAT directionX, FLOAT directionY)
{
	if (this->objects.link == NULL)
	{
		return;
	}
	ObjectManager * temp = this->objects.link;
	do
	{
		temp->moveStep(directionX, directionY);

		temp = temp->next;
	} while (temp && temp != this->objects.link);
}