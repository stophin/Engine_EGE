#pragma once

#include "Resources.h"
#include "LinkList.h"

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	LinkList<Resources> resource;
	VOID addResource(Resources * resource);
	Resources * createResource(const WCHAR * filename, INT resourceID, Resources * mask = NULL, INT cutmode = 0);
	Resources * getResource(INT resourceID);
	Resources * removeResource(INT resourceID);
};

