#pragma once

#include "Objects.h"
#include "LinkList.h"

typedef void(__cdecl * CallbackFunction)(void * lParam, void *wParam);

class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

	LinkList<Objects> object;

	ObjectManager * addStep(Objects * object, INT uniqueID);
	ObjectManager * addStep(Objects * object, Resources * resource, RectF & destination, RectF & truncation, INT uniqueID, FLOAT scale = 1);
	VOID removeStep(Objects * ibject);

	INT drawStep(Graphics & graphics);

	VOID moveStep(FLOAT directionX, FLOAT directionY);

	INT objectCount;
	CallbackFunction callbackFunction;
	VOID triggerFunction(void * lparam);


	INT uniqueID;

	ObjectManager * prev;
	ObjectManager * next;
};

