//QuadTree.h
//

#pragma once

#include "../extern/Roles.h"

extern class Roles;


class Rect2D {
public:
	Rect2D(EFTYPE x, EFTYPE y, EFTYPE w, EFTYPE h) :
		x(x), y(y), width(w), height(h){

	}
	Rect2D() :
		x(0), y(0), width(0), height(0) {

	}

	EFTYPE x;
	EFTYPE y;
	EFTYPE width;
	EFTYPE height;
	
	void set(EFTYPE x, EFTYPE y, EFTYPE  w, EFTYPE h) {
		this->x = x;
		this->y = y;
		this->width = w;
		this->height = h;
	}

	void set(const Rect2D& r) {
		this->x = r.x;
		this->y = r.y;
		this->width = r.width;
		this->height = r.height;
	}
};

enum QuadTree_Link {
	RightTop = 0,
	LeftTop = 1,
	LeftBottom = 2,
	RightBottom = 3,
	QuadTreeLinkMax = 4
};

#define MAX_QUARDANTS	4
#define MAX_OBJECTS		5
#define MAX_LEVELS		Role_Link::Max - Role_Link::End //reserved for normal linklist use
#define GET_LINK_INDEX(level)	(level + Role_Link::End - 1)
class QuadTree {
public:
	QuadTree(INT level, EFTYPE x, EFTYPE y, EFTYPE w, EFTYPE h,
		QuadTree * parent, INT position) :
		level(level),
		position(position),
		bounds(x, y, w, h),
		maxObjects(MAX_OBJECTS),
		maxLevels(MAX_LEVELS),
		objects(GET_LINK_INDEX(level)),
		parent(parent),
		hasChild(0){

	}
	QuadTree() :
		level(0),
		position(-1),
		maxObjects(MAX_OBJECTS),
		maxLevels(MAX_LEVELS),
		objects(GET_LINK_INDEX(level)),
		parent(NULL),
		hasChild(0) {

	}
	~QuadTree() {
		objects.~MultiLinkList();
	}

	INT level;
	INT position;
	Rect2D bounds;
	INT maxObjects;
	INT maxLevels;
	QuadTree * parent;
	INT hasChild;
	QuadTree * children[QuadTreeLinkMax];
	MultiLinkList<Roles> objects;

	//1 0 
	//2 3
	void Split() {
		EFTYPE subWidth = this->bounds.width / 2;
		EFTYPE subHeight = this->bounds.height / 2;
		EFTYPE x = this->bounds.x;
		EFTYPE y = this->bounds.y;

		this->children[QuadTree_Link::RightTop] =
			new QuadTree(this->level + 1, x + subWidth, y, subWidth, subHeight, this, QuadTree_Link::RightTop);
		this->children[QuadTree_Link::LeftTop] =
			new QuadTree(this->level + 1, x, y, subWidth, subHeight, this, QuadTree_Link::LeftTop);
		this->children[QuadTree_Link::LeftBottom] =
			new QuadTree(this->level + 1, x, y + subHeight, subWidth, subHeight, this, QuadTree_Link::LeftBottom);
		this->children[QuadTree_Link::RightBottom] =
			new QuadTree(this->level + 1, x + subWidth, y + subHeight, subWidth, subHeight, this, QuadTree_Link::RightBottom);
		this->hasChild = true;
	}

	INT GetIndex(Rect2D& rect) {
		INT index = -1;
		EFTYPE xMidPoint = this->bounds.x + this->bounds.width / 2;
		EFTYPE yMidPoint = this->bounds.y + this->bounds.height / 2;

		EFTYPE topQuad = (rect.y >= this->bounds.y && rect.y + rect.height <= yMidPoint);
		EFTYPE bottomQuad = (rect.y >= yMidPoint && rect.y + rect.height <= this->bounds.y + this->bounds.height);

		if (rect.x >= this->bounds.x && rect.x + rect.width <= xMidPoint) {//leftQuad
			if (topQuad) {
				index = 1;//leftTop
			}
			else if (bottomQuad) {
				index = 2;//leftBottom
			}
		}
		else if (rect.x >= xMidPoint && rect.x + rect.width <= this->bounds.x + this->bounds.width) {//rightQuad
			if (topQuad) {
				index = 0;//rightTop
			}
			else if (bottomQuad) {
				index = 3;//rightBottmFront
			}
		}
		return index;
	}

	Rect2D rect;
	void Insert(Roles * obj) {
		if (NULL == obj) {
			return;
		}
		if (this->hasChild) {
			rect.set(obj->flatting.X, obj->flatting.Y, obj->flatting.Width, obj->flatting.Height);
			INT index = this->GetIndex(rect);
			if (index != -1) {
				this->children[index]->Insert(obj);
			}
			else {
				this->objects.insertLink(obj);
				obj->quadTree = this;
			}
			return;
		}
		this->objects.insertLink(obj);
		obj->quadTree = this;
		if (this->objects.linkcount > this->maxObjects && this->level < this->maxLevels) {
			this->Split();
			Roles * _obj = this->objects.link, *_next;
			if (_obj) {
				do {
					_next = this->objects.next(_obj);

					rect.set(_obj->flatting.X, _obj->flatting.Y, _obj->flatting.Width, _obj->flatting.Height);
					INT index = this->GetIndex(rect);
					if (index != -1) {
						this->objects.removeLink(_obj);
						this->children[index]->Insert(_obj);
					}

					_obj = _next;
				} while (_obj && _obj != this->objects.link);
			}
		}
	}

	void Collision(Roles * obj, MultiLinkList<Roles> * link) {
		if (NULL == obj) {
			return;
		}
		if (NULL == link) {
			return;
		}
		if (this->hasChild) {
			rect.set(obj->flatting.X, obj->flatting.Y, obj->flatting.Width, obj->flatting.Height);
			INT index = this->GetIndex(rect);
			if (index != -1) {
				this->children[index]->Collision(obj, link);
			}
		}
		if (NULL == this->objects.link) {
			return;
		}
		Roles * _obj = this->objects.link;
		Relation::type relation;
		if (_obj) {
			do {

				if (_obj != obj) {
					relation = getRelation(_obj, obj);
					if (isRelationD(relation))
					{
						_obj->relation = relation;
						link->insertLink(_obj);
					}
				}

				_obj = this->objects.next(_obj);
			} while (_obj && _obj != this->objects.link);
		}
	}

	void Change(Roles * obj) {
		if (NULL == obj) {
			return;
		}
		if (NULL == obj->quadTree) {
			this->Insert(obj);
		}
		else
		{
			((QuadTree*)obj->quadTree)->objects.removeLink(obj);
			if (((QuadTree*)obj->quadTree)->parent) {
				((QuadTree*)obj->quadTree)->parent->Insert(obj);
			}
			else {
				this->Insert(obj);
			}
		}
	}
};