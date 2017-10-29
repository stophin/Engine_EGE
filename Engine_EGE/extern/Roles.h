#pragma once

#include "Relation.h"
#include "AnimationManager.h"
#include "Paths.h"

extern int DEBUG_MODE;
extern class World;

class Roles
{
public:
	Roles();
	~Roles();

	int count;

	union {
		struct {
			Roles * up;
			Roles * down;
			Roles * left;
			Roles * right;
		};
		Roles * dir[4];
	};

	union {
		struct {
			Roles * _up;
			Roles * _down;
			Roles * _left;
			Roles * _right;
		};
		Roles * _dir[4];
	};

	int attacked;
	int health;

	// Interactive role
	Roles * irole;
	// Interactive range
	PointF range;

	Animation_Base::type base;
	Animation_Direction::type direction;

	int scale;

	World * world;

	Role_Type::type type;
	Relation::type relation;
	AnimationManager animations;

	// TODO
	void * quadTree;

	int following;
	FLOAT tall;
	RectF position;
	RectF flatting;
	PointF leftTop;
	PointF vLeftBottom;
	PointF vRightBottom;
	PointF vRightTop;
	PointF oFlat;
	PointF oRole;

	int fetchSpeed;
	int fetchCount;
	int occupy;

	Paths * prevPath;
	Paths * nextPath;
	INT pathCount;
	MultiLinkList<Paths> paths;
	MultiLinkList<Paths> open;
	MultiLinkList<Paths> close;
	int moving;

	// TODO: AI
	Roles * srole;
	PointF speed;
	PointF eclose;

	void controlForce();

	///////////////////////////////////
	VOID astarPath(Paths * dest, INT limit = 100);

	VOID setFlatting(RectF & flatting, FLOAT tall);
	VOID updateFlatting();

	INT equal(Roles * role);

	INT drawAnimation(Graphics & graphics);

	VOID moveRole(FLOAT directionX, FLOAT directionY, INT mode = 0);
	VOID moveRole();
	VOID moveFlat(FLOAT directionX, FLOAT directionY, INT mode = 0);
	VOID moveFlat();
	VOID moveDelta(FLOAT dx, FLOAT dy);

	VOID center();

	VOID nextAnimation();
	Animations * selectAnimation(INT animationID);

	Animation_Direction::type getDirection(FLOAT dx, FLOAT dy);

	INT uniqueID;
	Roles * prev[Role_Link::Max];
	Roles * next[Role_Link::Max];
	void operator delete(void * _ptr);

	VOID fetchPath();
	VOID removePath();

	INT isin(Roles * role);
};

