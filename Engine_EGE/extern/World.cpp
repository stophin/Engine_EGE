
#include "World.h"
#include "Relation.h"
#include "Projection.h"
#include "Drawable.h"

extern int DEBUG_MODE;

World::World() :
roles(Role_Link::Role),
sorts(Role_Link::Sort),
objects(Role_Link::Object),
backs(Role_Link::Back),
fronts(Role_Link::Front),
players(Role_Link::Player),
sortsbacks(Role_Link::SortBacks),
astar(Role_Link::AStars),
focus(NULL),
cursor(NULL),
quadTree(NULL),
binTree(NULL)
{
}


World::~World()
{
}

VOID World::resize(FLOAT width, FLOAT height)
{
	this->geometry.Width = width;
	this->geometry.Height = height;

	this->leftTop = projectShow(this->geometry.X, this->geometry.Y);
	this->vLeftBottom = projectShow(this->geometry.X, this->geometry.Y + this->geometry.Height) - this->leftTop;
	this->vRightTop = projectShow(this->geometry.X + this->geometry.Width, this->geometry.Y) - this->leftTop;
	this->vRightBottom = projectShow(this->geometry.X + this->geometry.Width, this->geometry.Y + this->geometry.Height) - this->leftTop;
}

VOID World::offset(FLOAT offsetX, FLOAT offsetY)
{
	this->leftTop.X += offsetX;
	this->leftTop.Y += offsetY;

	PointF flat = projectFlat(this->leftTop.X, this->leftTop.Y);

	FLOAT flatX = flat.X - this->geometry.X;
	FLOAT flatY = flat.Y - this->geometry.Y;
	this->geometry.X = flat.X;
	this->geometry.Y = flat.Y;

	Roles * temp;

	if (this->roles.link)
	{
		temp = this->roles.link;
		do
		{
			temp->moveRole(offsetX, offsetY);

			if (!this->geometryCut(temp))
			{
				//this->sorts.insertLink(temp);
				this->refreshDepth(temp);
			}
			else
			{
				this->sorts.removeLink(temp);
			}

			temp = this->roles.next(temp);
		} while (temp && temp != this->roles.link);
	}

	this->refreshGeometryCut(flatX, flatY);
}

VOID World::offsetFlat(FLOAT offsetX, FLOAT offsetY)
{
	this->geometry.X += offsetX;
	this->geometry.Y += offsetY;

	this->leftTop = projectShow(this->geometry.X, this->geometry.Y);

	Roles * temp;
	if (this->roles.link)
	{
		temp = this->roles.link;
		do
		{
			temp->moveFlat(offsetX, offsetY);

			if (!this->geometryCut(temp))
			{
				//this->sorts.insertLink(temp);
				this->refreshDepth(temp);
			}
			else
			{
				this->sorts.removeLink(temp);
			}

			temp = this->roles.next(temp);
		} while (temp && temp != this->roles.link);
	}

	this->refreshGeometryCut(offsetX, offsetY);
}
INT compare(Roles * temp, Roles * link) {
	int ret = 0;
	if (DEF_ISZERO(link->flatting.Y - temp->flatting.Y)) {
		if (link->flatting.X > temp->flatting.X) {
			ret = 1;
		}
	}
	return ret;
}

VOID World::addRole(Roles * role, Role_Type::type type)
{
	role->type = type;

	if (this->roles.link == NULL)
	{
		this->focus = role;
	}
	if (role->type != Role_Type::Back) {
		this->roles.insertLink(role);
	}
	role->world = this;

	switch (type)
	{
	case Role_Type::Normal:
		this->sorts.insertLink(role);
		this->refreshDepth(role, 0);
		//this->refreshGeometryCut();
		this->objects.insertLink(role);
		/*
		if (this->quadTree) {
			this->quadTree.insert(role);
		}
		if (this->binTree) {
			this->binTree.insertNode(role);
		}
		*/
		break;
	case Role_Type::Player:
		this->players.insertLink(role);
		this->objects.insertLink(role);

		/*
		if (this->quadTree) {
			this->quadTree.insert(role);
		}
		if (this->binTree) {
			this->binTree.insertNode(role);
		}
		*/
		break;
	case Role_Type::Cursor:
		this->fronts.insertLink(role);
		this->cursor = role;
		break;
	default:
		this->backs.insertSortF(role, compare);
		if (!this->geometryCut(role))
		{
			this->insertSortsBacks(role, compare);
		}
	}

	//bind world to each role's animation's object
	Animations * animation = role->animations.animation.link;
	if (animation) {
		do {
			ObjectManager * objectManager = animation->objects.link;
			if (objectManager) {
				do {
					Objects * object = objectManager->object.link;
					if (object) {
						do {
							object->world = this;

							object = object->next;
						} while (object && object != objectManager->object.link);
					}

					objectManager = objectManager->next;
				} while (objectManager && objectManager != animation->objects.link);
			}

			animation = animation->next;
		} while (animation && animation != role->animations.animation.link);
	}
}

Roles * World::removeRole(Roles * role)
{
	if (role == NULL)
	{
		return NULL;
	}
	if (this->roles.removeLink(role) == NULL)
	{
		return NULL;
	}
	switch (role->type)
	{
	case Role_Type::Normal:
		this->objects.removeLink(role);
		break;
	case Role_Type::Cursor:
		this->fronts.removeLink(role);
		this->cursor = NULL;
		break;
	case Role_Type::Player:
		this->players.removeLink(role);
		break;
	default:
		this->backs.removeLink(role);
		this->removeSortsBacks(role);
	}
	this->sorts.removeLink(role);
	role->world = NULL;

	if (role->equal(this->focus))
	{
		this->focus = NULL;
	}

	role->type = Role_Type::Disabled;

	return role;
}

Roles * World::drawRole(Graphics & graphics)
{
	Roles * cur = NULL;

	if (DEBUG_MODE & DEBUG_GRADE_4)
	{
		SolidBrush brush(Color(255, 255, 0, 0));
		FontFamily fontFamily(L"Ó×Ô²");
		Gdiplus::Font font(&fontFamily, 12, FontStyleRegular, UnitPixel);
		CHAR fmt[100];
		WCHAR str[100];

		sprintf_s(fmt, "%d", this->sorts.linkcount);
		wsprintfW(str, L"%S", fmt);
		graphics.DrawString(str, -1, &font, PointF(10, 330), &brush);
	}

	INT i = 0;
	Roles * temp = NULL;

	if (0 && this->players.link)
	{
		temp = this->players.link;
		do
		{
			temp->drawAnimation(graphics);

			i++;
			temp = this->players.next(temp);
		} while (temp && temp != this->players.link);
	}
	if (this->fronts.link)
	{
		temp = this->fronts.prev(this->fronts.link);
		do
		{
			temp->drawAnimation(graphics);
			i++;
			temp = this->fronts.prev(temp);
		} while (temp && temp != this->fronts.prev(this->fronts.link));
	}
	if (this->sorts.link)
	{
		temp = this->sorts.prev(this->sorts.link);
		do
		{
			if (temp->drawAnimation(graphics) && !cur) {
				cur = temp;
			}

			if (DEBUG_MODE & DEBUG_GRADE_4)
			{
				SolidBrush brush(Color(255, 255, 0, 0));
				FontFamily fontFamily(L"¨]¨o¨\¨ˆ");
				Gdiplus::Font font(&fontFamily, 12, FontStyleRegular, UnitPixel);
				CHAR fmt[100];
				WCHAR str[100];

				sprintf_s(fmt, "%p", temp);
				wsprintfW(str, L"%S", fmt);
				graphics.DrawString(str, -1, &font, PointF(10, 350 + i++ * 10), &brush);
			}

			i++;

			if (temp->moving) {
				temp->nextAnimation();
			}
			temp = this->sorts.prev(temp);
		} while (temp && temp != this->sorts.prev(this->sorts.link));
	}
	if (this->sortsbacks.link)
	{
		temp = this->sortsbacks.prev(this->sortsbacks.link);
		do
		{
			temp->drawAnimation(graphics);

			i++;
			temp = this->sortsbacks.prev(temp);
		} while (temp && temp != this->sortsbacks.prev(this->sortsbacks.link));
	}

	SolidBrush brush(Color(255, 255, 0, 0));
	FontFamily fontFamily(L"Ó×Ô²");
	Gdiplus::Font font(&fontFamily, 12, FontStyleRegular, UnitPixel);
	CHAR fmt[100];
	WCHAR str[100];

	sprintf_s(fmt, "%d", i);
	wsprintfW(str, L"%S", fmt);
	graphics.DrawString(str, -1, &font, PointF(10, 350), &brush);

	return cur;
}

VOID World::refreshRole()
{
	if (this->sorts.link == NULL)
	{
		return;
	}
	Roles * temp = this->sorts.link;
	do
	{
		temp->nextAnimation();

		temp = this->sorts.next(temp);
	} while (temp && temp != this->sorts.link);

}

void World::insertSortsBacks(Roles * role, MultiLinkList<Roles>::COMPARE_FUN fun) {
	/*
	for (int i = 0; i < 4; i++) {
		if (role->dir[i]) {
			int j = 0;
			switch (i) {
			case 0: j = 1; break;
			case 1: j = 0; break;
			case 2: j = 3; break;
			case 3: j = 2; break;
			}
			role->_dir[i] = role->dir[i];
			role->dir[i]->_dir[j] = role;
		}
	}
	*/

	this->sortsbacks.insertSortF(role, fun);
}
void World::removeSortsBacks(Roles * role) {
	/*
	for (int i = 0; i < 4; i++) {
		if (role->_dir[i]) {
			int j = 0;
			switch (i) {
			case 0: j = 1; break;
			case 1: j = 0; break;
			case 2: j = 3; break;
			case 3: j = 2; break;
			}
			role->_dir[i]->_dir[j] = NULL;
		}
		role->_dir[i] = NULL;
	}
	*/

	this->sortsbacks.removeLink(role);
}

VOID World::refreshGeometryCut(FLOAT flatX, FLOAT flatY)
{
	Roles * temp = NULL, *_temph = NULL, *_tempw = NULL, *next = NULL;
	this->count = 0;
	temp = this->backs.link;
	if (temp) {
		do {
			temp->moveFlat(flatX, flatY, 1);

			temp = this->backs.next(temp);
		} while (temp && temp != this->backs.link);
	}
	if (this->sortsbacks.link) {
		MultiLinkList<Roles> tempLink(Role_Link::Temp1);
		temp = this->sortsbacks.link;
		int layw = (int)(abs(flatX) / temp->flatting.Width + 2);
		int layh = (int)(abs(flatY) / temp->flatting.Height + 2);
		int i, j, k;
		do
		{
			next = this->sortsbacks.next(temp);
			//if (temp->_up == NULL ||temp->_down == NULL ||temp->_left == NULL ||temp->_left == NULL)
			{

				_temph = temp;
				for (i = 0; i <= layh; i++) {
					if (_temph && _temph->down) {
						_temph = _temph->down;
					}
					else {
						break;
					}
					this->count++;
				}
				for (i = -layh; i <= layh; i++) {
					if (_temph) {
						_tempw = _temph;
						for (j = 0; j <= layw; j++) {
							if (_tempw && _tempw->left) {
								_tempw = _tempw->left;
							}
							else {
								break;
							}
							this->count++;
						}
						for (k = -layw; k <= layw; k++) {
							if (_tempw) {
								if (!tempLink.next(_tempw)) {
									//tempLink.insertSortF(_tempw, compare);
									tempLink.insertLink(_tempw);
								}
								_tempw->moveFlat();

								_tempw = _tempw->right;
							}
							else {
								break;
							}
							this->count++;
						}
						_temph = _temph->up;
					}
					else {
						break;
					}
					this->count++;
				}
			}
			temp->moveFlat();

			temp = next;
		} while (temp && temp != this->sortsbacks.link);
		this->count = 0;
		temp = tempLink.link;
		if (temp) {
			do {

				if (this->geometryCut(temp))
				{
					this->removeSortsBacks(temp);
				}
				else
				{
					//if (!this->sortsbacks.isin(temp))
					if (!this->sortsbacks.next(temp))
					{
						this->insertSortsBacks(temp, compare);
					}
					else {
						temp = temp;
					}
				}
				this->count++;
				temp = tempLink.next(temp);
			} while (temp && temp != tempLink.link);
		}
		tempLink.clearLink();
	}
	else {
		if (this->backs.link)
		{
			temp = this->backs.link;
			do
			{
				temp->moveFlat();

				if (this->geometryCut(temp))
				{
					this->removeSortsBacks(temp);
				}
				else
				{
					//if (!this->sortsbacks.isin(temp))
					if (!this->sortsbacks.next(temp))
					{
						this->insertSortsBacks(temp, compare);
					}
				}
				this->count++;

				temp = this->backs.next(temp);
			} while (temp && temp != this->backs.link);
		}
	}
}

MultiLinkList<Roles> * World::getCollision(Roles * item, MultiLinkList<Roles> * collision)
{
	if (collision == NULL)
	{
		return collision;
	}
	if (item == NULL)
	{
		return collision;
	}
	if (item->type != Role_Type::Normal && item->type != Role_Type::Player)
	{
		return collision;
	}
	/*
	if (this->quadTree)
	{
		this->quadTree.move(item);
		this->quadTree.collision(item, collision, graphics);
		return collision;
	}
	*/
	if (this->objects.link == NULL)
	{
		return collision;
	}
	Roles * tango = this->objects.link;
	Relation::type relation;
	do
	{
		if (tango != item)
		{
			relation = getRelation(tango, item);
			if (isRelationD(relation))
			{
				tango->relation = relation;
				collision->insertLink(tango);
			}
		}
		tango = this->objects.next(tango);
	} while (tango && tango != this->objects.link);
	return collision;
}

MultiLinkList<Roles> * World::refreshDepth(Roles * item, MultiLinkList<Roles> * collision)
{
	if (!collision) {
		collision = NULL;
	}
	if (item == NULL)
	{
		return collision;
	}
	if (this->sorts.link == NULL)
	{
		this->sorts.insertLink(item);
		return collision;
	}
	this->sorts.removeLink(item);
	if (this->sorts.link == NULL)
	{
		this->sorts.insertLink(item);
		return collision;
	}
	if (item->type != Role_Type::Normal && item->type != Role_Type::Player)
	{
		return collision;
	}
	Roles * before = NULL;
	Roles * after = NULL;
	MultiLinkList<Roles> * linkList = &this->sorts;
	/*
	if (0 && item->quadTree) {
		linkList = item->quadTree.objects;
	}
	*/
	Roles * tango = linkList->link;
	if (tango == NULL)
	{
		return collision;
	}
	Relation::type relation;
	do
	{
		relation = getRelation(tango, item);
		tango->relation = relation;
		if (isRelationD(relation))
		{
			if (collision)
			{
				collision->insertLink(tango);
			}
			if (tango->position.Y + tango->tall > item->position.Y + item->tall)
			{
				//this->sorts.insertLink(item, tango);
				before = tango;
				break;
			}
			//else
			//{
				//this->sorts.insertLink(item, NULL, tango);
				//after = tango;
			//}
		}
		else if (relation == Relation::A)
		{
			//this->sorts.insertLink(item, tango);
			before = tango;
			break;
		}
		//else if (relation == Relation::B || relation == Relation::C)
		//{
			//this->sorts.insertLink(item, NULL, tango);
			//after = tango;
		//}

		tango = linkList->next(tango);
	} while (tango && tango != linkList->link);
	
	this->sorts.insertLink(item, before, after);

	return collision;
}

INT World::geometryCut(Roles * item)
{
	FLOAT px = item->position.X;
	FLOAT itemW = item->position.Width;

	FLOAT bondX = 10;

	if (px + itemW < bondX + this->display.X)
	{
		return 1;
	}
	if (px > this->geometry.Width - bondX + this->display.X)
	{
		return 1;
	}
	FLOAT py = item->position.Y;
	FLOAT itemH = item->position.Height;
	FLOAT bondY = 10;
	if (py + itemH < bondY + this->display.Y)
	{
		return 1;
	}
	if (py > this->geometry.Height - bondY + this->display.Y)
	{
		return 1;
	}
	return 0;
}