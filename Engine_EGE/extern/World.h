#pragma once

#include "MultiLinkList.h"
#include "Roles.h"

class World
{
public:
	int count;
	World();
	~World();

	PointF display;
	RectF geometry;
	PointF leftTop;
	PointF vLeftBottom;
	PointF vRightBottom;
	PointF vRightTop;
	VOID resize(FLOAT width, FLOAT height);
	VOID offset(FLOAT offsetX, FLOAT offsetY);
	VOID offsetFlat(FLOAT offsetX, FLOAT offsetY);
	
	Roles * focus;
	Roles * cursor;

	MultiLinkList<Roles> roles;		//Roles, Except the Land
	MultiLinkList<Roles> sorts;		//Sort Link
	MultiLinkList<Roles> objects;	// All the objects
	MultiLinkList<Roles> backs;		// Land
	MultiLinkList<Roles> sortsbacks;		// Land's sorts
	MultiLinkList<Roles> fronts;	// Front objects, cursor, etc.
	MultiLinkList<Roles> players;	// Players, dynamic
	MultiLinkList<Roles> astar;		// Astars
	void insertSortsBacks(Roles * role, MultiLinkList<Roles>::COMPARE_FUN fun);
	void removeSortsBacks(Roles * role);

	void * quadTree;
	void * binTree;
	
	VOID addRole(Roles * role, Role_Type::type type = Role_Type::Normal);
	Roles * removeRole(Roles * role);
	Roles * drawRole(Graphics & grapchis);
	VOID refreshRole();

	VOID refreshGeometryCut(FLOAT flatX, FLOAT flatY);
	INT geometryCut(Roles * item);
	MultiLinkList<Roles> * refreshDepth(Roles *item, MultiLinkList<Roles> * collision = NULL);
	MultiLinkList<Roles> * getCollision(Roles * item, MultiLinkList<Roles> * collision);
};
