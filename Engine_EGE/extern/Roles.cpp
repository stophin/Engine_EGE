
#include "Roles.h"

#include "Drawable.h"
#include "World.h"
#include "Paths.h"

#include "../NanoCGR.h"
extern NanoCGR nanoCGR;

Roles::Roles() :
count(0),
attacked(0),
up(NULL),
down(NULL),
left(NULL),
right(NULL),
_up(NULL),
_down(NULL),
_left(NULL),
_right(NULL),
health(DEF_HEALTH),
irole(NULL),
range(DEF_RANGE_W, DEF_RANGE_H),
base(Animation_Base::Still),
direction(Animation_Direction::Down),
scale(1),
type(Role_Type::Normal),
world(NULL),
relation(Relation::A),
animations(),
quadTree(NULL),
following(0),
tall(0),
fetchSpeed(DEF_FETCH_SPEED),
fetchCount(0),
occupy(DEF_OCCUPY),
prevPath(NULL),
nextPath(NULL),
pathCount(0),
paths(Path_Link::Path),
open(Path_Link::Open),
close(Path_Link::Close),
moving(0),
srole(NULL)
{
	for (INT i = 0; i < Role_Link::Max; i++)
	{
		this->prev[i] = NULL;
		this->next[i] = NULL;
	}
}

Roles::~Roles()
{
	this->animations.~AnimationManager();
}

void Roles::operator delete(void * _ptr)
{
	if (_ptr == NULL)
	{
		return;
	}
	for (INT i = 0; i < Role_Link::Max; i++)
	{
		if (((Roles*)_ptr)->prev[i] != NULL || ((Roles*)_ptr)->next[i] != NULL)
		{
			return;
		}
	}
	delete(_ptr);
}

INT Roles::equal(Roles * role)
{
	if (role == NULL)
	{
		return 0;
	}
	return (this->uniqueID == role->uniqueID);
}

INT Roles::drawAnimation(Graphics & graphics)
{
	INT in = this->animations.drawAnimation(graphics);
	
	if ((this->world && this == this->world->focus) || DEBUG_MODE & DEBUG_GRADE_1)
	{

		if (this->type == Role_Type::Normal)
		{
			if (0)
			{
				CHAR fmt[100];
				EP_SetColor(WHITE);
				sprintf_s(fmt, "(%.2f, %.2f, %.2f, %.2f)", this->flatting.X, this->flatting.Y, this->flatting.Width, this->flatting.Height);
				EP_Text(this->position.X, this->position.Y, fmt);
				EP_SetColor(GREEN);
				sprintf_s(fmt, "(%.2f, %.2f, %.2f, %.2f)", this->position.X, this->position.Y, this->position.Width, this->position.Height);
				EP_Text(this->position.X, this->position.Y + 20, fmt);
			}
			EP_SetColor(GREEN);
			EP_Rectangle(this->position);

			EP_SetColor(BLUE);
			EP_Rectangle(this->flatting);

			EP_SetColor(YELLOW);
			EP_Rectangle(this->animations.animation.link->objects.link->object.link->destination);
		}
	}

	if (DEBUG_MODE & DEBUG_GRADE_3)
	{
		if (this->type == Role_Type::Normal || this->type == Role_Type::Player)
		{
			EP_SetColor(RED);
			PointF points[] = { this->leftTop, this->leftTop + this->vRightTop, this->leftTop + this->vRightBottom, this->leftTop + this->vLeftBottom, this->leftTop };

			EP_Lines(points, 5);
		}
	}

	if (DEBUG_MODE & DEBUG_GRADE_4)
	{
		CHAR fmt[100];
		EP_SetColor(WHITE);
		stringRelation(this->relation, fmt);
		EP_Text(this->position.X, this->position.Y, fmt);
	}
	return in;
}

VOID Roles::setFlatting(RectF & flatting, FLOAT tall)
{
	this->tall = tall;
	this->flatting = flatting;

	this->updateFlatting();
}

VOID Roles::updateFlatting()
{
	this->leftTop = projectShow(this->flatting.X, this->flatting.Y);
	this->vLeftBottom = projectShow(this->flatting.X, this->flatting.Y + this->flatting.Height) - this->leftTop;
	this->vRightTop = projectShow(this->flatting.X + this->flatting.Width, this->flatting.Y) -this->leftTop;
	this->vRightBottom = projectShow(this->flatting.X + this->flatting.Width, this->flatting.Y + this->flatting.Height) - this->leftTop;

	FLOAT directionX = this->position.X;
	FLOAT directionY = this->position.Y;

	this->position.X = this->leftTop.X + this->vLeftBottom.X;
	this->position.Y = this->leftTop.Y + this->vRightBottom.Y -this->tall;
	this->position.Width = this->vRightTop.X - this->vLeftBottom.X;
	this->position.Height = this->tall;

	this->animations.moveAnimation(this->position.X - directionX, this->position.Y - directionY);
}

VOID Roles::moveRole(FLOAT directionX, FLOAT directionY, INT mode)
{
	this->oRole.X += directionX;
	this->oRole.Y += directionY;
	if (!mode) {
		this->moveRole();
	}
}

VOID Roles::moveRole()
{
	if (DEF_ISZERO(this->oRole.X) && DEF_ISZERO(this->oRole.Y)) {
		return;
	}
	FLOAT directionX = this->oRole.X;
	FLOAT directionY = this->oRole.Y;
	this->oRole.X = 0;
	this->oRole.Y = 0;

	this->position.X += directionX;
	this->position.Y += directionY;
	this->animations.moveAnimation(directionX, directionY);

	directionX = this->flatting.X;
	directionY = this->flatting.Y;

	PointF point = projectFlat(this->position.X - this->vLeftBottom.X, this->position.Y + this->tall - this->vRightBottom.Y);
	this->flatting.X = point.X;
	this->flatting.Y = point.Y;

	directionX -= this->flatting.X;
	directionY -= this->flatting.Y;
	
	this->leftTop = projectShow(this->flatting.X, this->flatting.Y);

}

VOID Roles::moveFlat(FLOAT directionX, FLOAT directionY, INT mode)
{
	this->oFlat.X += directionX;
	this->oFlat.Y += directionY;
	if (!mode) {
		this->moveFlat();
	}
}

void Roles::moveFlat() {
	if (DEF_ISZERO(this->oFlat.X) && DEF_ISZERO(this->oFlat.Y)) {
		return;
	}
	FLOAT directionX = this->oFlat.X;
	FLOAT directionY = this->oFlat.Y;
	this->oFlat.X = 0;
	this->oFlat.Y = 0;

	this->flatting.X += directionX;
	this->flatting.Y += directionY;

	this->leftTop = projectShow(this->flatting.X, this->flatting.Y);

	directionX = this->position.X;
	directionY = this->position.Y;

	this->position.X = this->leftTop.X + this->vLeftBottom.X;
	this->position.Y = this->leftTop.Y + this->vRightBottom.Y - this->tall;

	this->animations.moveAnimation(this->position.X - directionX, this->position.Y - directionY);
}

VOID Roles::nextAnimation()
{
	this->animations.nextAnimation(this, this->moving);
}

Animations * Roles::selectAnimation(INT animationID)
{
	if (this->animations.animation.link->uniqueID == animationID)
	{
		return this->animations.animation.link;
	}
	Animations * animation = this->animations.animation.getLink(animationID);
	if (animation != NULL)
	{
		this->animations.animation.link = animation;
	}
	return animation;
}

VOID Roles::astarPath(Paths * dest, INT limit)
{
	if (this->world) {
		// add into astar list
		if (this->world->astar.next(this) == NULL) {
			this->world->astar.insertLink(this, NULL, NULL);
		}
	}

	this->removePath();

	Paths * start = new Paths(0, 0);
	start->f = 0;
	open.insertLink(start);
	dest->f = Astar_Infinity;
	open.insertSort(dest);

	Paths pt_l(0, 0);
	Paths pt_r(0, 0);
	Paths pt_t(0, 0);
	Paths pt_b(0, 0);
	Paths pt_lt(0, 0);
	Paths pt_lb(0, 0);
	Paths pt_rt(0, 0);
	Paths pt_rb(0, 0);
	Paths * pts[] = { &pt_l, &pt_r, &pt_t, &pt_b, &pt_lt, &pt_lb, &pt_rt, &pt_rb };

	Paths * t_pt, *t_po, *t_pc;
	while (open.linkcount > 0)
	{
		Paths * pt = open.link;
		open.removeLink(pt);
		close.insertLink(pt);

		if (open.linkcount > limit || close.linkcount > limit)
		{
			close.~MultiLinkList();
			open.~MultiLinkList();
			break;
		}
		if (dest->atPointXY(RectF(pt->x, pt->y, 1, 1)))
		{
			this->paths.insertLink(dest);
			Paths * temp = pt->parent;
			while (temp)
			{
				this->paths.insertLink(temp);

				temp = temp->parent;
			}

			this->paths.removeLink(start);
			this->paths.insertLink(start);

			this->pathCount = this->paths.linkcount;

			close.~MultiLinkList();
			open.~MultiLinkList();
			break;
		}

		pt_l.set(pt->x - 1, pt->y);
		pt_r.set(pt->x + 1, pt->y);
		pt_t.set(pt->x, pt->y - 1);
		pt_b.set(pt->x, pt->y + 1);
		pt_lt.set(pt->x - 1, pt->y - 1);
		pt_lb.set(pt->x - 1, pt->y + 1);
		pt_rt.set(pt->x + 1, pt->y - 1);
		pt_rb.set(pt->x + 1, pt->y + 1);

		// chage max i among 4/8
		for (int i = 0; i < 8; i++)
		{
			t_pt = pts[i];
			t_pt->getF(this, dest);
			t_pt->parent = pt;

			//t_pc = close.isin(t_pt);
			t_pc = close.next(t_pt);
			if (!t_pc)
			{
				//t_po = open.isin(t_pt);
				t_po = open.next(t_pt);
				if (!t_po)
				{
					open.insertSort(new Paths(*t_pt));
				}
				else if (t_pt->f < t_po->f)
				{
					//(*t_po) = (*t_pt);
					t_po->f = t_pt->f;
					t_po->parent = t_pt->parent;

					open.removeLink(t_po);
					open.insertSort(t_po);
				}
			}
			else
			{
				if (t_pt->f < t_pc->f)
				{
					close.removeLink(t_pc);
					(*t_pc) = (*t_pt);
					open.insertSort(t_pc);
				}
			}
		}

	}
}

VOID Roles::removePath()
{
	if (this->nextPath)
	{
		delete this->nextPath;
		this->nextPath = NULL;
	}
	if (this->prevPath)
	{
		delete this->prevPath;
		this->prevPath = NULL;
	}

	this->paths.~MultiLinkList();
	this->pathCount = 0;
}

VOID Roles::fetchPath()
{
	if (this->nextPath == NULL)
	{
		this->nextPath = this->paths.removeLink(this->paths.prev(this->paths.link));
		if (this->prevPath == NULL)
		{
			this->prevPath = this->nextPath;
			this->nextPath = this->paths.removeLink(this->paths.prev(this->paths.link));
		}
	}
	if (this->nextPath == NULL)
	{
		if (this->prevPath)
		{
			delete this->prevPath;
			this->prevPath = NULL;
		}
		// remove from astar list
		if (!this->moving && this->world) {
			this->world->astar.removeLink(this);
		}
		this->moving = 0;
		return;
	}
	if (this->fetchCount++ >= this->fetchSpeed)
	{
		// When first in path, focus role
		if (this->following && this->paths.linkcount == this->pathCount - 2)
		{
			this->center();
		}

		this->fetchCount = 0;
		if (this -> prevPath)
		{
			delete this->prevPath;
			this->prevPath = NULL;
		}
		this->prevPath = this->nextPath;
		this->nextPath = NULL;
	}
	else
	{
		FLOAT dx = (this->nextPath->x - this->prevPath->x) / (FLOAT)this->fetchSpeed;
		FLOAT dy = (this->nextPath->y - this->prevPath->y) / (FLOAT) this->fetchSpeed;

		this->moveDelta(dx, dy);
	}
}

void Roles::moveDelta(FLOAT dx, FLOAT dy) {

		Animation_Base::type oldbase = this->base;
		this->base = Animation_Base::Move;
		
		Animation_Direction::type olddirection = this->direction;
		this->direction = this->getDirection(dx * this->flatting.Width, dy * this->flatting.Height);
		INT animationID = this->base + this->direction;
		if (animationID >= 0)
		{
			this->selectAnimation(animationID);
		}
		
		this->moveFlat(dx * this->flatting.Width, dy * this->flatting.Height);
		this->world->refreshDepth(this);

		if (1) {
			this->world->changeQuadTree(this);
		}
		
		char temp[100];
		sprintf_s(temp, "Position: %.2f, %.2f", (this->flatting.X - this->world->geometry.X), (this->flatting.Y - this->world->geometry.Y));
		nanoCGR.Send(temp);

		if (this->following)
		{
			this->world->offsetFlat(-dx * this->flatting.Width, -dy * this->flatting.Height);
		}
		
		this->moving = 1;
}
Animation_Direction::type Roles::getDirection(FLOAT dx, FLOAT dy) {
	Animation_Direction::type animationID = Animation_Direction::Down;

	if (dx == 0)
	{
		dx = DEF_ZERO;
	}
	FLOAT h = atan(-(FLOAT)dy / (FLOAT)dx);

	if (-dy > 0)
	{
		if (dx < 0)
		{
			h += PI;
		}
	}
	else
	{
		if (dx >= 0)
		{
			h += 2 * PI;
		}
		else if (dx < 0)
		{
			h += PI;
		}
	}

	if (h >= PI / 8)
	{
		if (h < 3 * PI / 8)
		{
			animationID = Animation_Direction::Right;
			this->speed.X = 1;
			this->speed.Y = -1;
		}
		else if (h < 5 * PI / 8)
		{
			animationID = Animation_Direction::Right_Up;
			this->speed.X = 0;
			this->speed.Y = -1;
		}
		else if (h < 7 * PI / 8)
		{
			animationID = Animation_Direction::Up;
			this->speed.X = -1;
			this->speed.Y = -1;
		}
		else if (h < 9 * PI / 8)
		{
			animationID = Animation_Direction::Left_Up;
			this->speed.X = -1;
			this->speed.Y = 0;
		}
		else if (h < 11 * PI / 8)
		{
			animationID = Animation_Direction::Left;
			this->speed.X = -1;
			this->speed.Y = 1;
		}
		else if (h < 13 * PI / 8)
		{
			animationID = Animation_Direction::Left_Down;
			this->speed.X = 0;
			this->speed.Y = 1;
		}
		else if (h < 15 * PI / 8)
		{
			animationID = Animation_Direction::Down;
			this->speed.X = 1;
			this->speed.Y = 1;
		}
		else
		{
			animationID = Animation_Direction::Right_Down;
			this->speed.X = 1;
			this->speed.Y = 0;
		}
	}
	else
	{
		animationID = Animation_Direction::Right_Down;
		this->speed.X = 0;
		this->speed.Y = 1;
	}

	return animationID;
}

void Roles::center() {
	if (!this->world) {
		return;
	}
	this->world->offset(-(this->position.X - this->world->geometry.Width / 2 - this->world->display.X), -(this->position.Y - this->world->geometry.Height / 2 - this->world->display.Y));
}

INT Roles::isin(Roles* role)
{
	return this == role;
}