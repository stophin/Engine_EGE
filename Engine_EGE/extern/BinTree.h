//BinTree.h
//

#pragma once

#include "../extern/Roles.h"

extern class Roles;

class BinNode {
public:
	BinNode(Roles * role) :
		role(role),
		left(NULL),
		right(NULL),
		parent(NULL),
		color(RB_Color::Black),
		value(role->uniqueID){

	}
	
	Roles * role;
	INT value;

	BinNode * left;
	BinNode * right;
	BinNode * parent;

	INT color;

	BinNode * uncle() {
		if (this->parent == this->grandPa()->left) {
			return this->grandPa()->right;
		}
		return this->grandPa()->left;
	}

	BinNode * grandPa() {
		return this->parent->parent;
	}
};

class BinTree {
public:
	BinTree():
		header(NULL){

	}

	BinNode * header;

	void insertNode(Roles * role) {
		if (NULL == role) {
			return;
		}
		BinNode * node = new BinNode(role);
		BinNode * target_pos = this->header;
		BinNode * parent = NULL;
		INT be_in_left = 1;

		while (target_pos) {
			parent = target_pos;
			//insert into right tree
			if (this->compare(node, target_pos)) {
				target_pos = target_pos->right;
				be_in_left = 0;
			}
			//insert into left tree
			else {
				target_pos = target_pos->left;
				be_in_left = 1;
			}
		}

		node->parent = parent;
		if (parent) {
			if (be_in_left) {
				parent->left = node;
			}
			else {
				parent->right = node;
			}
		}
		else {
			this->header = node;
		}

		this->adjustPath(node);
	}

	Roles * getRole(INT key) {
		BinNode * temp = this->getNode(key);
		if (temp) {
			return temp->role;
		}
		return NULL;
	}

	BinNode * getNode(INT key) {
		return this->traverse(this->header, key);
	}

	BinNode * traverse(BinNode * node, INT key) {
		if (NULL == node) {
			return NULL;
		}
		if (node->value == key) {
			return node;
		}
		if (key > node->value) {
			return this->traverse(node->right, key);
		}
		else {
			return this->traverse(node->left, key);
		}
	}

	INT compare(BinNode * l, BinNode * r) {
		if (NULL == l || NULL == r) {
			return 0;
		}
		if (l->value > r->value) {
			return 1;
		}
		return 0;
	}

	void adjustPath(BinNode * node) {
		if (NULL == node) {
			return;
		}
		if (NULL == node->parent) {
			return;
		}
		if (NULL == node->grandPa()) {
			return;
		}
		if (NULL == this->header) {
			return;
		}
		BinNode * w = NULL;
		while (node != this->header && RB_Color::Red == node->parent->color) {
			//left tree
			if (node->parent == node->grandPa()->left) {
				w = node->uncle();
				//if uncle is Red then put uncle and parent to Black
				//and grandpa to Red, if grandpa's parent might be
				//Red, which need to be adjusted later
				if (NULL != w && RB_Color::Red == w->color) {
					w->color = RB_Color::Black;
					node->parent->color = RB_Color::Black;
					node->grandPa()->color = RB_Color::Red;
					node = node->grandPa();
				}
				else {
					//right tree
					if (node == node->parent->right) {
						node = node->parent;
						this->rotate_left(node);
					}
					node->parent->color = RB_Color::Black;
					node->grandPa()->color = RB_Color::Red;

					this->rotate_right(node->grandPa());
				}
			}
			//right tree
			else {
				w = node->uncle();
				//if uncle is Red then put uncle and parent to Black
				//and grandpa to Red, if grandpa's parent might be
				//Red, which need to be adjusted later
				if (NULL != w && RB_Color::Red == w->color) {
					w->color = RB_Color::Black;
					node->parent->color = RB_Color::Black;
					node->grandPa()->color = RB_Color::Red;
					node = node->grandPa();
				}
				else {
					//right tree
					if (node == node->parent->left) {
						node = node->parent;
						this->rotate_right(node);
					} 
					node->parent->color = RB_Color::Black;
					node->grandPa()->color = RB_Color::Red;

					this->rotate_left(node->grandPa());
				}
			}
		}
		this->header->color = RB_Color::Black;
	}

	void rotate_left(BinNode * node) {
		if (NULL == node) {
			return;
		}
		BinNode * parent = node->parent;
		BinNode * right = node->right;
		if (NULL == right) {
			return;
		}
		BinNode * rleft = right->left;
		//put node as right tree's left tree
		//and right tree's left tree as node's right tree
		right->left = node;
		node->parent = right;
		node->right = rleft;
		//replace node with right tree
		if (NULL != parent) {
			//if it's original left tree
			if (node == parent->left) {
				parent->left = right;
			}
			else {
				parent->left = right;
			}
		}
		right->parent = parent;
		//put right tree's original left tree into the root
		if (rleft) {
			rleft->parent = node;
		}
		if (NULL == parent) {
			this->header = right;
		}
	}

	void rotate_right(BinNode * node) {
		if (NULL == node) {
			return;
		}
		BinNode * parent = node->parent;
		BinNode * left = node->left;
		if (NULL == left) {
			return;
		}
		BinNode * lright = left->right;

		//put node as left tree's right tree
		left->right = node;
		node->parent = left;
		node->left = lright;
		//replace node with left tree
		if (NULL != parent) {
			//if it's original left tree
			if (node == parent->left) {
				parent->left = left;
			}
			else {
				parent->right = left;
			}
		}
		left->parent = parent;

		//put left tree's original right tree into the root
		if (lright) {
			lright->parent = node;
		}
		if (NULL == parent) {
			this->header = left;
		}
	}
};
