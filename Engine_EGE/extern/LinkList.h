#pragma once

#include "../flatform/EPaint.h"

template <typename T>
class LinkList
{
public:
	LinkList();
	~LinkList();

	T * link;
	INT linkcount;

	VOID insertLink(T * link, T * before = NULL, T * after = NULL);
	VOID removeLink(T * link);
	T * getLink(INT uniqueID);
};



template<typename T>
LinkList<T>::LinkList() :
linkcount(0),
link(NULL)
{
}

template<typename T>
LinkList<T>::~LinkList()
{
	if (this->link)
	{
		T * temp = this->link;
		do
		{
			removeLink(temp);

			delete temp;

			temp = this->link;
		} while (temp);
	}
}

template<typename T>
VOID LinkList<T>::insertLink(T * link, T * before, T * after)
{
	if (this->link == NULL)
	{
		this->link = link;

		this->link->prev = link;
		this->link->next = link;

		this->linkcount = this->linkcount + 1;

		return;
	}
	else
	{
		if (before == NULL && after == NULL)
		{
			before = this->link;
			after = this->link->prev;
		}
		else if (before == NULL)
		{
			before = after->next;
		}
		else if (after == NULL)
		{
			after = before->prev;
		}
		else /* before != NULL && after != NULL*/
		{
			if (before->prev != after || after->next != before)
			{
				return;
			}
		}

		link->prev = after;
		link->next = before;
		after->next = link;
		before->prev = link;

		this->linkcount = this->linkcount + 1;
	}
}

template<typename T>
VOID LinkList<T>::removeLink(T * link)
{
	if (link->prev == NULL || link->next == NULL)
	{
		return;
	}
	T * before = link->prev;
	T * after = link->next;

	before->next = after;
	after->prev = before;
	link->prev = NULL;
	link->next = NULL;

	if (this->link == link)
	{
		this->link = before;
	}
	if (this->link == link)
	{
		this->link = NULL;
	}

	this->linkcount = this->linkcount - 1;
}

template<typename T>
T * LinkList<T>::getLink(INT uniqueID)
{
	if (this->link == NULL)
	{
		return NULL;
	}
	T * temp = this->link;
	do
	{
		if (temp->uniqueID == uniqueID)
		{
			break;
		}
		temp = temp->next;
	} while (temp && temp != this->link);
	return temp;
}