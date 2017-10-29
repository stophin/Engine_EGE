
#include "ObjectManager.h"

ObjectManager::ObjectManager():
callbackFunction(NULL),
objectCount(0)
{
}

ObjectManager::~ObjectManager()
{
}

ObjectManager * ObjectManager::addStep(Objects * object, INT uniqueID)
{
	object->uniqueID = uniqueID;
	this->object.insertLink(object);

	return this;
}

ObjectManager * ObjectManager::addStep(Objects * object, Resources * resource, RectF & destination, RectF & truncation, INT uniqueID, FLOAT scale)
{
	object->resource = resource;
	object->destination = destination;
	object->truncation = truncation;

	object->destination.Width *= scale;
	object->destination.Height *= scale;
	object->destination.X *= scale;
	object->destination.Y *= scale;

	this->addStep(object, uniqueID);

	return this;
}

VOID ObjectManager::removeStep(Objects * object)
{
	this->object.removeLink(object);
}

INT ObjectManager::drawStep(Graphics & graphics)
{
	if (this->object.link == NULL)
	{
		return 0;
	}
	return this->object.link->drawObject(graphics);
}

VOID ObjectManager::moveStep(FLOAT directionX, FLOAT directionY)
{
	if (this->object.link == NULL)
	{
		return;
	}
	Objects * temp = this->object.link;
	do
	{
		temp->moveObject(directionX, directionY);

		temp = temp->next;
	} while (temp && temp != this->object.link);
}

VOID ObjectManager::triggerFunction(void * lParam)
{
	this->objectCount++;
	if (this->callbackFunction && lParam && this->objectCount >= this->object.linkcount)
	{
		this->objectCount = 0;
		this->callbackFunction(lParam, this);
	}
	if (this->objectCount < 0)
	{
		this->objectCount = 0;
	}
}