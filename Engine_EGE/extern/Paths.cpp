
#include "Paths.h"
#include "Roles.h"
#include "MultiLinkList.h"
#include "World.h"

Paths::Paths(FLOAT x, FLOAT y):
x(x),
y(y),
f(0),
parent(NULL),
collisions(Role_Link::Collision)
{
	for (INT i = 0; i < Path_Link::Max; i++)
	{
		this->prev[i] = NULL;
		this->next[i] = NULL;
	}
}

Paths::Paths(const Paths & path):
x(path.x),
y(path.y),
f(path.f),
parent(path.parent),
collisions(Role_Link::Collision),
uniqueID(path.uniqueID)
{
	for (INT i = 0; i < Path_Link::Max; i++)
	{
		this->prev[i] = path.prev[i];
		this->next[i] = path.next[i];
	}
}
void Paths::operator =(const Paths & path)
{
	this->x = path.x;
	this->y = path.y;
	this->f = path.f;
	this->parent = path.parent;
}

void Paths::operator delete(void * _ptr)
{
	if (_ptr == NULL)
	{
		return;
	}
	for (INT i = 0; i < Path_Link::Max; i++)
	{
		if (((Paths*)_ptr)->prev[i] != NULL || ((Paths*)_ptr)->next[i] != NULL)
		{
			return;
		}
	}
	delete(_ptr);
}
Paths::~Paths()
{
}

VOID Paths::set(FLOAT x, FLOAT y)
{
	this->x = x;
	this->y = y;
}

extern Graphics * graphics;

VOID Paths::getF(Roles * role, Paths * path)
{
	this->f = 0;

	FLOAT x = this->x * role->flatting.Width;FLOAT y = this->y * role->flatting.Height;

	collisions.clearLink();
	role->moveFlat(x, y);
	Roles * cursor = role->world->cursor;
	FLOAT o_x, o_y;
	if (cursor)
	{
		o_x = cursor->flatting.X;
		o_y = cursor->flatting.Y;
		cursor->moveFlat(role->flatting.X - cursor->flatting.X, role->flatting.Y - cursor->flatting.Y);
		cursor->drawAnimation(*graphics);
	}
	role->world->getCollision(role, &collisions);
	if (collisions.link && collisions.linkcount > 0)
	{
		Roles * collision = collisions.link;
		do
		{
			this->f += collision->occupy;

			collision = collisions.next(collision);
		} while (collision && collision != collisions.link);
	}
	role->moveFlat(-x, -y);
	collisions.clearLink();
	if (cursor)
	{
		cursor->moveFlat(o_x - cursor->flatting.X, o_y - cursor->flatting.Y);
		cursor->drawAnimation(*graphics);
	}

	this->f += abs((LONG)(this->x - path->x)) + abs((LONG)(this->y - path->y));
}

INT Paths::atPointXY(RectF & rect)
{
	if (this->x > rect.X - rect.Width && this->x < rect.X + rect.Width &&
		this->y > rect.Y - rect.Height && this->y < rect.Y + rect.Height)
	{
		return 1;
	}
	return 0;
}

INT Paths::isin(Paths * path)
{
	return this->atPointXY(RectF(path->x, path->y, 1, 1));
}