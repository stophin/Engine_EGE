//EImage.cpp
//
//
//Author: Stophin
//2014.01.08
//Ver: 0.01


#include "EImage.h"

EImage::EImage():
geom(),
image(NULL),
image_bk(NULL),
imgtype(0),
factor(DEF_GEOM_PAM),
shift(DEF_GEOM_PAM),
tall(DEF_GEOM_PAM),
regX(DEF_GEOM_PAM),
regY(DEF_GEOM_PAM)
{
}

EImage::~EImage()
{
	if (image)
	{
		EP_DeleteImage(image);
		image=NULL;
	}
	if (0 && image_bk)
	{
		EP_DeleteImage(image_bk);
		image_bk=NULL;
	}

}

EPVOID EImage::SetGeom(EPTYPE x,EPTYPE y)
{
	if (x>=0&&y>=0)
	{
		geom.Set(x,y);
		this->regX = geom.x;
		this->regY = geom.y;
	}
	return;
}

EPVOID EImage::ResetGeom(EPTYPE x,EPTYPE y)
{
	if (x>=0&&y>=0)
	{
		geom.Set(x,y);
	}
	EP_ResizeImage(image,x,y);
	EP_ResizeImage(image_bk,x,y);
}


EPTYPE EImage::Width()
{
	return geom.x;
}

EPTYPE EImage::Height()
{
	return geom.y;
}

EPVOID EImage::LoadImage(EPSTRING fimg,EPSTRING fimg_bk)
{
	EP_LoadImage(image,fimg,geom.x,geom.y);
	EP_LoadImage(image_bk,fimg_bk,geom.x,geom.y);
}

EPVOID EImage::LoadImageFrnt(EPSTRING fimg)
{
	EP_LoadImage(image,fimg,geom.x,geom.y);
}

EPVOID EImage::LoadImageBack(EPSTRING fimg_bk)
{
	EP_LoadImage(image_bk,fimg_bk,geom.x,geom.y);
}
