#pragma once

#include "Resources.h"
#include "MultiLinkList.h"

extern class Roles;

class Paths
{
public:
	Paths(FLOAT x, FLOAT y);
	Paths(const Paths & path);
	~Paths();

	MultiLinkList<Roles> collisions;

	Paths * parent;

	FLOAT x;
	FLOAT y;
	FLOAT f;
	VOID set(FLOAT x, FLOAT y);

	INT uniqueID;
	Paths * prev[Path_Link::Max];
	Paths * next[Path_Link::Max];
	void operator =(const Paths & path);
	void operator delete(void * _ptr);

	VOID getF(Roles * role, Paths * path);
	INT atPointXY(RectF & rect);
	INT isin(Paths * path);
};

