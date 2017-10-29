
#include "ResourceManager.h"


ResourceManager::ResourceManager()
{
}


ResourceManager::~ResourceManager()
{
}

VOID ResourceManager::addResource(Resources * resource)
{
	this->resource.insertLink(resource);
}

Resources * ResourceManager::createResource(const WCHAR * filename, INT resourceID, Resources * mask, INT cutmode)
{
	return new Resources(filename, resourceID, mask, cutmode);
}

Resources * ResourceManager::getResource(INT resourceID)
{
	return this->resource.getLink(resourceID);
}

Resources * ResourceManager::removeResource(INT resourceID)
{
	Resources * resource = this->resource.getLink(resourceID);
	if (resource == NULL)
	{
		return NULL;
	}
	this->resource.removeLink(resource);
	return resource;
}