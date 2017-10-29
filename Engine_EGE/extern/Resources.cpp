
#include "Resources.h"


Resources::Resources(const WCHAR * filename, INT resourceID, Resources * mask, INT cutmode):
cutmode(cutmode)
{
	this->uniqueID = resourceID;
	this->image = new EImage();
	if (this->image)
	{
		this->image->SetGeom(100, 100);
		//this->image->LoadImageW(filename, "1");
		//EP_LoadImage(this->image->image, filename, this->image->geom.x, this->image->geom.y);
		this->image->image = newimage();
		ege::getimage(this->image->image, filename);
		if (mask)
		{
			this->image->image_bk = mask->image->image;
		}
		this->width = (FLOAT)this->image->Width();
		this->height = (FLOAT)this->image->Height();
	}
}


Resources::~Resources()
{
	if (this->image)
	{
		delete this->image;
		this->image = NULL;
	}
}
